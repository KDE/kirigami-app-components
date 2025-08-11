// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2025 Marco Martin <notmart@gmail.com>


#include "actioncollection.h"

#include <QSettings>
#include <QKeySequence>
#include <QQmlProperty>

#include <KConfig>
#include <KSharedConfig>
#include <KConfigGroup>

Q_GLOBAL_STATIC(ActionCollectionStorage, s_actionCollectionStorage)

ActionCollectionData::ActionCollectionData(QObject *parent)
    : QObject(parent)
{
}

ActionCollectionData::~ActionCollectionData()
{}

QString ActionCollectionData::collection() const
{
    return m_collection;
}

void ActionCollectionData::setCollection(const QString &collection)
{
    if (m_collection == collection)
        return;

    m_collection = collection;

    if (!parent()->objectName().isEmpty()) {
        ActionCollectionStorage::self()->collection(collection)->addAction(parent()->objectName(), parent());
    }

    Q_EMIT collectionChanged();
}

ActionCollectionData *ActionCollectionData::qmlAttachedProperties(QObject *object)
{
    return new ActionCollectionData(object);
}

//////////////////////////////////

ActionCollection::ActionCollection(const QString &name, QObject *parent)
    : QObject(parent)
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig(QStringLiteral(":/actionsrc"));
    KConfigGroup rootCg(config, name);

    for (const QString &grp : rootCg.groupList()) {
        KConfigGroup cg(&rootCg, grp);
        ActionData a;
        a.icon = cg.readEntry("icon", QString());
        a.shortcut = cg.readEntry("shortcut", QString());
        m_actionData[grp] = a;
        qWarning() << grp << a.icon << a.shortcut;
    }
}

ActionCollection::~ActionCollection()
{}

QString ActionCollection::name() const
{
    return m_name;
}

void ActionCollection::addAction(const QString &name, QObject *action)
{
    if (!action) {
        return;
    }
    qWarning() << "adding action" << name << action;

    int position = -1;

    for (const QString &n : m_actions.keys()) {
        ++position;
        if (n > name) {
            break;
        }
    }
    Q_EMIT aboutToAddAction(position, action);

    m_actions[name] = action;

    ActionData data = m_actionData.value(name);
    if (!data.icon.isEmpty()) {
        QQmlProperty property(action, "icon.name");
        property.write(data.icon);
    }
    if (data.shortcut.isEmpty()) {
        return;
    }

    QMetaEnum me = QMetaEnum::fromType<QKeySequence::StandardKey>();

    // Is the default shortcut a named one, such as "Copy", "Back" etc ?
    for (int i = 0; i < me.keyCount(); ++i) {
        if (QString::fromUtf8(me.key(i)).toLower() == data.shortcut.toLower()) {
            QQmlProperty property(action, "shortcut");
            property.write(QKeySequence::StandardKey(me.value(i)));
            return;
        }
    }

    connect(action, &QObject::destroyed, this, [this] () {
        QObject *action = sender();
        const int position = m_actions.keys().indexOf(action->objectName());
        Q_EMIT aboutToRemoveAction(position, action);
        m_actions.remove(action->objectName());
        Q_EMIT actionRemoved(position, action);
    });

    QQmlProperty property(action, "shortcut");
    property.write(data.shortcut);
    Q_EMIT actionAdded(position, action);
}

QObject *ActionCollection::action(const QString &name)
{
    return m_actions.value(name);
}

QList<QObject *> ActionCollection::actions() const
{
    return m_actions.values();
}

QString ActionCollection::defaultShortcut(const QString &name) const
{
    return m_actionData.value(name).shortcut;
}

/////////////////////////////////

ActionCollectionModel::ActionCollectionModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

ActionCollectionModel::~ActionCollectionModel()
{}

QString ActionCollectionModel::name() const
{
    if (m_collection) {
        return m_collection->name();
    }

    return {};
}

void ActionCollectionModel::setName(const QString &name)
{
    if (m_collection && name == m_collection->name()) {
        return;
    }

    m_collection = ActionCollectionStorage::self()->collection(name);

    connect(m_collection, &ActionCollection::aboutToAddAction,
            this, [this](int position, QObject *action) {
                Q_UNUSED(position);
                beginInsertRows(QModelIndex(), position, position);
            });
    connect(m_collection, &ActionCollection::actionAdded,
            this, [this](int position, QObject *action) {
                Q_UNUSED(position);
                Q_UNUSED(action);
                endInsertRows();
            });
    connect(m_collection, &ActionCollection::aboutToRemoveAction,
            this, [this](int position, QObject *action) {
                Q_UNUSED(position);
                beginRemoveRows(QModelIndex(), position, position);
            });
    connect(m_collection, &ActionCollection::actionAdded,
            this, [this](int position, QObject *action) {
                Q_UNUSED(position);
                Q_UNUSED(action);
                endRemoveRows();
            });

    Q_EMIT nameChanged(m_collection ? name : QString());
}

QObject *ActionCollectionModel::action(const QString &name)
{
    if (!m_collection) {
        return nullptr;
    }

    return m_collection->action(name);
}

ActionCollection *ActionCollectionModel::collection() const
{
    return m_collection;
}

int ActionCollectionModel::rowCount(const QModelIndex &parent) const
{
    if (!m_collection) {
        return 0;
    }

    return m_collection->actions().count();
}

QVariant ActionCollectionModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(checkIndex(index, QAbstractItemModel::CheckIndexOption::IndexIsValid));

    if (!m_collection) {
        return {};
    }

    QObject *action = m_collection->actions()[index.row()];

    switch (role) {
    case Qt::DisplayRole:
        return action->property("text");
    case IconNameRole: {
        QQmlProperty property(action, "icon.name");
        return property.read();
    }
    case DefaultShortcutRole:
        return m_collection->defaultShortcut(action->objectName());
    case ShortcutRole: {
        QVariant shortcutVariant = action->property("shortcut");
        if (shortcutVariant.canConvert<int>()) {
            QKeySequence seq(QKeySequence::StandardKey(shortcutVariant.value<int>()));
            return seq.toString();
        }
        return shortcutVariant;
    }
    }

    return {};
}

QHash<int, QByteArray> ActionCollectionModel::roleNames() const
{
    return {
        { Qt::DisplayRole, "display" },
        { IconNameRole, "iconName" },
        { DefaultShortcutRole, "defaultShortcut" },
        { ShortcutRole, "shortcut" },
        { ShortcutDisplayRole, "shortcutDisplay" },
        { AlternateShortcutsRole, "alternateShortcuts" },
        { CollectionNameRole, "collectionName" },
    };
}

/////////////////////////////////

ActionCollectionStorage::ActionCollectionStorage(QObject *parent)
    : QObject(parent)
{}

ActionCollectionStorage::~ActionCollectionStorage()
{}

ActionCollectionStorage *ActionCollectionStorage::self()
{
    return s_actionCollectionStorage;
}

ActionCollection *ActionCollectionStorage::collection(const QString &name)
{
    ActionCollection *coll = nullptr;
    if (!m_collections.contains(name)) {
        coll = new ActionCollection(name, this);
        m_collections[name] = coll;
        return coll;
    }

    return m_collections[name];
}


#include "moc_actioncollection.cpp"
