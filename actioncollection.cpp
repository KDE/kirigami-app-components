// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2025 Marco Martin <notmart@gmail.com>


#include "actioncollection.h"

#include <QSettings>
#include <QKeySequence>
#include <QQmlProperty>

#include <KConfig>
#include <KSharedConfig>
#include <KConfigGroup>
#include <KLocalizedString>

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
        ActionCollectionStorage::self()->collection(collection)->addActionInstance(parent()->objectName(), parent());
    }

    Q_EMIT collectionChanged();
}

ActionCollectionData *ActionCollectionData::qmlAttachedProperties(QObject *object)
{
    return new ActionCollectionData(object);
}

//////////////////////////////////

QString decodeStandardShortcut(const QString &shortcut)
{
    // Is the default shortcut a named one, such as "Copy", "Back" etc ?
    QMetaEnum me = QMetaEnum::fromType<QKeySequence::StandardKey>();
    for (int i = 0; i < me.keyCount(); ++i) {
        if (QString::fromUtf8(me.key(i)).toLower() == shortcut) {
            return QKeySequence(QKeySequence::StandardKey(me.value(i))).toString();
        }
    }

    return shortcut;
}

ActionCollection::ActionCollection(const QString &name, QObject *parent)
    : QObject(parent)
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig(QStringLiteral(":/actionsrc"));
    KConfigGroup rootCg(config, name);
    KConfigGroup savedShortcutsCg(KSharedConfig::openConfig(), "Shortcuts");

    int i = 0;
    for (const QString &grp : rootCg.groupList()) {
        KConfigGroup cg(&rootCg, grp);
        ActionData ad;
        ad.name = grp;
        ad.text = cg.readEntry("text", i18n("Action %1", i));
        ad.icon = cg.readEntry("icon", QString());
        ad.defaultShortcut = cg.readEntry("shortcut", QString());
        ad.shortcut = savedShortcutsCg.readEntry(grp, ad.defaultShortcut);

        m_actionData[grp] = ad;
        qWarning() << grp << ad.icon << ad.shortcut;
        ++i;
    }
}

ActionCollection::~ActionCollection()
{}

QString ActionCollection::name() const
{
    return m_name;
}

void ActionCollection::addActionInstance(const QString &name, QObject *action)
{
    if (!action) {
        return;
    }
    qWarning() << "adding action instance" << name << action;

    int position = -1;

    for (const QString &n : m_actions.keys()) {
        ++position;
        if (n > name) {
            break;
        }
    }
    //FIXME: rethink how actions can be dinamically added/removed
    bool knownAction = m_actionData.contains(name);
    if (!knownAction) {
        Q_EMIT aboutToAddAction(position, action);
        ActionData ad;
        ad.text = action->property("text").toString();
        QQmlProperty property(action, "icon.name");
        ad.icon = property.read().toString();
        ad.shortcut = action->property("shortcut").toString();
        m_actionData[name] = ad;
    }

    m_actions[name] = action;

    ActionData data = m_actionData.value(name);
    if (!data.icon.isEmpty()) {
        QQmlProperty property(action, "icon.name");
        property.write(data.icon);
    }
    if (data.shortcut.isEmpty()) {
        return;
    }

    /*
    connect(action, &QObject::destroyed, this, [this] () {
        QObject *action = sender();
        const int position = m_actions.keys().indexOf(action->objectName());
        Q_EMIT aboutToRemoveAction(position, action);
        m_actions.remove(action->objectName());
        Q_EMIT actionRemoved(position, action);
    });*/

    QQmlProperty property(action, "shortcut");
    property.write(decodeStandardShortcut(data.shortcut));
    if (!knownAction) {
        Q_EMIT actionAdded(position, action);
    }
}

QObject *ActionCollection::actionInstance(const QString &name)
{
    return m_actions.value(name);
}

QList<QObject *> ActionCollection::actions() const
{
    return m_actions.values();
}

QList<ActionData > ActionCollection::actionDescriptions() const
{
    return m_actionData.values();
}

void ActionCollection::setShortcut(const QString &name, const QString &shortcut)
{
    if (shortcut == ActionCollection::shortcut(name)) {
        return;
    }
    QString actualShortcut = shortcut;
    KConfigGroup cg(KSharedConfig::openConfig(), "Shortcuts");
    if (shortcut.isEmpty() || shortcut == decodeStandardShortcut(defaultShortcut(name))) {
        cg.deleteEntry(name);
        actualShortcut = decodeStandardShortcut(defaultShortcut(name));
    } else {
        cg.writeEntry(name, actualShortcut);
    }

    QObject *action = m_actions.value(name);
    if (action) {
        action->setProperty("shortcut", actualShortcut);
    }

    Q_EMIT shortcutChanged(actualShortcut);
}

QString ActionCollection::shortcut(const QString &name) const
{
    KConfigGroup cg(KSharedConfig::openConfig(), "Shortcuts");
    return cg.readEntry(name, m_actionData.value(name).shortcut);
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

void ActionCollectionModel::setShortcut(const QString &name, const QString &shortcut)
{
    if (!m_collection) {
        return;
    }
    m_collection->setShortcut(name, shortcut);
}

ActionCollection *ActionCollectionModel::collection() const
{
    return m_collection;
}

int ActionCollectionModel::rowCount(const QModelIndex &parent) const
{
    if (!m_collection || parent.isValid()) {
        return 0;
    }

    return m_collection->actionDescriptions().count();
}

QVariant ActionCollectionModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(checkIndex(index, QAbstractItemModel::CheckIndexOption::IndexIsValid));

    if (!m_collection) {
        return {};
    }

    ActionData ad = m_collection->actionDescriptions()[index.row()];

    switch (role) {
    case Qt::DisplayRole:
        return ad.text;
    case ActionNameRole:
        return ad.name;
    case IconNameRole:
        return ad.icon;
    case DefaultShortcutRole:
        return decodeStandardShortcut(ad.defaultShortcut);
    case ShortcutRole:
        return decodeStandardShortcut(ad.shortcut);
    }

    return {};
}

QHash<int, QByteArray> ActionCollectionModel::roleNames() const
{
    return {
        { Qt::DisplayRole, "display" },
        { ActionNameRole, "actionName" },
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
