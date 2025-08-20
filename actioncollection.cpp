// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2025 Marco Martin <notmart@gmail.com>


#include "actioncollection.h"
#include "actiondata.h"

#include <QSettings>
#include <QKeySequence>
#include <QQmlProperty>

#include <KConfig>
#include <KSharedConfig>
#include <KConfigGroup>
#include <KLocalizedString>

Q_GLOBAL_STATIC(ActionCollectionStorage, s_actionCollectionStorage)

ActionCollectionAttached::ActionCollectionAttached(QObject *parent)
    : QObject(parent)
{
}

ActionCollectionAttached::~ActionCollectionAttached()
{}

QString ActionCollectionAttached::collection() const
{
    return m_collection;
}

void ActionCollectionAttached::setCollection(const QString &collection)
{
    if (m_collection == collection)
        return;

    m_collection = collection;

    ActionCollection *coll = ActionCollectionStorage::self()->collection(collection);
    const QString name = parent()->objectName();

    if (coll && !name.isEmpty()) {
        ActionData *ad = coll->action(name);
        qWarning()<<ad;
        if (ad) {
            ad->setAction(parent());
        }
    }

    Q_EMIT collectionChanged();
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

ActionCollection::ActionCollection(QObject *parent)
    : QObject(parent)
{
    /*
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
    }*/
}

ActionCollection::~ActionCollection()
{}

QString ActionCollection::name() const
{
    return m_name;
}

void ActionCollection::setName(const QString &name)
{
    if (name == m_name) {
        return;
    }

    m_name = name;

    ActionCollectionStorage::self()->insertCollection(this);

    Q_EMIT nameChanged(name);
}

ActionData *ActionCollection::action(const QString &name)
{
    return m_actionMap.value(name);
}

QList<ActionData *> ActionCollection::actions() const
{
    return m_actions;
}

QList<ActionData *> ActionCollection::activeActions() const
{
    //TODO
    return m_actions;
}

ActionCollectionAttached *ActionCollection::qmlAttachedProperties(QObject *object)
{
    return new ActionCollectionAttached(object);
}

void ActionCollection::actions_append(QQmlListProperty<ActionData> *prop, ActionData *action)
{
    // This can only be called from QML
    ActionCollection *coll = static_cast<ActionCollection *>(prop->object);
    if (!coll) {
        return;
    }

    const QString name = action->name();
    if (coll->m_actionMap.contains(name)) {
        qmlEngine(coll)->throwError(i18n("Action name %1 not unique.", name));
    }
    coll->m_actions.append(action);
    coll->m_actionMap[name] = action;

    connect(action, &QObject::destroyed, coll, [coll, action, name]() {
        coll->m_actionMap.remove(name);
        coll->m_actions.removeAll(action);
    });

    action->setParent(coll);
}

qsizetype ActionCollection::actions_count(QQmlListProperty<ActionData> *prop)
{
    ActionCollection *coll = static_cast<ActionCollection *>(prop->object);
    if (!coll) {
        return 0;
    }

    return coll->m_actions.count();
}

ActionData *ActionCollection::actions_at(QQmlListProperty<ActionData> *prop, qsizetype index)
{
    ActionCollection *coll = static_cast<ActionCollection *>(prop->object);
    if (!coll) {
        return nullptr;
    }

    if (index < 0 || index >= coll->m_actions.count()) {
        return nullptr;
    }
    return coll->m_actions.value(index);
}

void ActionCollection::actions_clear(QQmlListProperty<ActionData> *prop)
{
    ActionCollection *coll = static_cast<ActionCollection *>(prop->object);
    if (!coll) {
        return;
    }

    coll->m_actionMap.clear();
    coll->m_actions.clear();
}

QQmlListProperty<ActionData> ActionCollection::actionsProperty()
{
    return QQmlListProperty<ActionData>(this,
                                        nullptr,
                                        actions_append,
                                        actions_count,
                                        actions_at,
                                        actions_clear);
}

/////////////////////////////////
/*
ActionsModel::ActionsModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

ActionsModel::~ActionsModel()
{}

QString ActionsModel::name() const
{
    if (m_collection) {
        return m_collection->name();
    }

    return {};
}

void ActionsModel::setName(const QString &name)
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

ActionCollection *collModel::collection() const
{
    return m_collection;
}

int ActionsModel::rowCount(const QModelIndex &parent) const
{
    if (!m_collection || parent.isValid()) {
        return 0;
    }

    return m_collection->actionDescriptions().count();
}

QVariant ActionsModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(checkIndex(index, QAbstractItemModel::CheckIndexOption::IndexIsValid));

    if (!m_collection) {
        return {};
    }

    QObject *action = m_collection->actions()[index.row()];

    switch (role) {
    case Qt::DisplayRole:
        return action->property("text");
    case ActionInstanceRole:
        return QVariant::fromValue(action);
    }

    return {};
}

QHash<int, QByteArray> ActionsModel::roleNames() const
{
    return {
        { Qt::DisplayRole, "display" },
        { ActionInstanceRole, "actionInstance" }
    };
}
*/
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

void ActionCollectionStorage::insertCollection(ActionCollection *collection)
{
    m_collections.insert(collection->name(), collection);
}

ActionCollection *ActionCollectionStorage::collection(const QString &name)
{
    return m_collections.value(name);
}


#include "moc_actioncollection.cpp"
