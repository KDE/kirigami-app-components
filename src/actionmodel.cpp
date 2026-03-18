// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2025 Marco Martin <notmart@gmail.com>

#include "actionmodel.h"
#include "actioncollections_p.h"
#include "actiondata.h"
#include "qmlactioncollection_p.h"

#include <QKeySequence>
#include <QPersistentModelIndex>
#include <QQmlContext>
#include <QQmlProperty>
#include <QtQml/qqmlinfo.h>

class ActionModelPrivate
{
public:
    ActionModelPrivate(ActionModel *model);
    void connectCollection(QmlActionCollection *collection);
    qsizetype collectionPosition(QmlActionCollection *collection) const;
    QList<QmlActionCollection *> collections() const;

    ActionModel *q;
    QString m_collectionName;
    QHash<ActionData *, QKeySequence> m_pendingShortcuts;
    QHash<ActionData *, QKeySequence> m_pendingAlternateShortcuts;
    ActionModel::ShownActions m_shownActions = ActionModel::AllActions;
};

ActionModelPrivate::ActionModelPrivate(ActionModel *model)
    : q(model)
{
}

void ActionModelPrivate::connectCollection(QmlActionCollection *collection)
{
    QObject::connect(collection, &QmlActionCollection::aboutToAddActionInstance, q, [this, collection](int position, QObject *action) {
        Q_UNUSED(action)
        if (m_shownActions != ActionModel::ActiveActions) {
            return;
        }
        const int cp = collectionPosition(collection);
        q->beginInsertRows(QModelIndex(), cp + position, cp + position);
    });
    QObject::connect(collection, &QmlActionCollection::actionInstanceAdded, q, [this](int position, QObject *action) {
        Q_UNUSED(position)
        Q_UNUSED(action)
        if (m_shownActions != ActionModel::ActiveActions) {
            return;
        }
        q->endInsertRows();
    });
    QObject::connect(collection, &QmlActionCollection::aboutToRemoveActionInstance, q, [this](int position, QObject *action) {
        Q_UNUSED(action)
        if (m_shownActions != ActionModel::ActiveActions) {
            return;
        }
        q->beginRemoveRows(QModelIndex(), position, position);
    });
    QObject::connect(collection, &QmlActionCollection::actionInstanceRemoved, q, [this](int position, QObject *action) {
        Q_UNUSED(position);
        Q_UNUSED(action);
        if (m_shownActions != ActionModel::ActiveActions) {
            return;
        }
        q->endRemoveRows();
    });
}

qsizetype ActionModelPrivate::collectionPosition(QmlActionCollection *collection) const
{
    qsizetype collectionPosition = 0;

    const QList<QmlActionCollection *> &collections = ActionCollections::self()->d->m_collections.values();

    for (auto *coll : collections) {
        if (coll == collection) {
            break;
        }
        if (m_shownActions == ActionModel::ActiveActions) {
            collectionPosition += coll->activeActions().count();
        } else {
            collectionPosition += coll->actions().count();
        }
    }

    return collectionPosition;
}

QList<QmlActionCollection *> ActionModelPrivate::collections() const
{
    return ActionCollections::self()->d->m_collections.values();
}

////////////////////////////////////////

ActionModel::ActionModel(QObject *parent)
    : QAbstractListModel(parent)
    , d(new ActionModelPrivate(this))
{
    const QList<QmlActionCollection *> &collections = d->collections();

    for (auto *coll : collections) {
        d->connectCollection(coll);
    }
    connect(ActionCollections::self(), &ActionCollections::collectionInserted, this, [this](ActionCollection *collection) {
        d->connectCollection(qobject_cast<QmlActionCollection *>(collection));
    });
}

ActionModel::~ActionModel()
{
}

QString ActionModel::collectionName() const
{
    return d->m_collectionName;
}

void ActionModel::setCollectionName(const QString &name)
{
    return;
    if (d->m_collectionName == name) {
        return;
    }

    d->m_collectionName = name;

    Q_EMIT collectionNameChanged(name);
}

ActionModel::ShownActions ActionModel::shownActions() const
{
    return d->m_shownActions;
}

void ActionModel::setShownActions(ShownActions shown)
{
    if (shown == d->m_shownActions) {
        return;
    }

    beginResetModel();
    d->m_shownActions = shown;
    endResetModel();

    Q_EMIT shownActionsChanged(shown);
}

void ActionModel::save()
{
    for (auto it = d->m_pendingShortcuts.constBegin(); it != d->m_pendingShortcuts.constEnd(); ++it) {
        it.key()->setVariantShortcut(it.value());
    }

    for (auto it = d->m_pendingAlternateShortcuts.constBegin(); it != d->m_pendingAlternateShortcuts.constEnd(); ++it) {
        it.key()->setVariantAlternateShortcut(it.value());
    }

    d->m_pendingShortcuts.clear();
    d->m_pendingAlternateShortcuts.clear();
}

void ActionModel::reset(const QModelIndex &index)
{
    ActionData *action = index.data(ActionDescriptionRole).value<ActionData *>();
    if (!action) {
        return;
    }

    bool found = false;
    auto it = d->m_pendingShortcuts.constFind(action);
    if (it != d->m_pendingShortcuts.constEnd()) {
        d->m_pendingShortcuts.erase(it);
        found = true;
    }
    it = d->m_pendingAlternateShortcuts.constFind(action);
    if (it != d->m_pendingAlternateShortcuts.constEnd()) {
        d->m_pendingAlternateShortcuts.erase(it);
        found = true;
    }

    if (found) {
        Q_EMIT dataChanged(index, index, {ShortcutRole, AlternateShortcutRole});
    }
}

void ActionModel::resetAll()
{
    if (d->m_pendingShortcuts.isEmpty() && d->m_pendingAlternateShortcuts.isEmpty()) {
        return;
    }
    beginResetModel();
    d->m_pendingShortcuts.clear();
    d->m_pendingAlternateShortcuts.clear();
    endResetModel();
}

int ActionModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    const QList<QmlActionCollection *> &collections = d->collections();

    if (d->m_shownActions == ActiveActions) {
        return std::accumulate(collections.constBegin(), collections.constEnd(), 0, [](int sum, QmlActionCollection *coll) {
            return sum + coll->actions().count();
        });
    }

    return std::accumulate(collections.constBegin(), collections.constEnd(), 0, [](int sum, QmlActionCollection *coll) {
        return sum + coll->actions().count();
    });
}

QVariant ActionModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(checkIndex(index, QAbstractItemModel::CheckIndexOption::IndexIsValid));

    qsizetype actualRow = index.row();
    QmlActionCollection *collection = nullptr;
    ActionData *action;
    const QList<QmlActionCollection *> &collections = d->collections();
    if (d->m_shownActions == ActiveActions) {
        for (auto *coll : collections) {
            if (coll->activeActions().count() > actualRow) {
                collection = coll;
                break;
            }
            actualRow -= coll->activeActions().count();
        }
    } else {
        for (auto *coll : collections) {
            if (coll->actions().count() > actualRow) {
                collection = coll;
                break;
            }
            actualRow -= coll->actions().count();
        }
    }
    if (!collection) {
        return {};
    }
    action = collection->activeActions()[actualRow];
    switch (role) {
    case Qt::DisplayRole:
        return action->text();
    case ActionDescriptionRole:
        return QVariant::fromValue(action);
    case ActionInstanceRole:
        return QVariant::fromValue(action->action());
    case ActionCollectionRole:
        return collection->text();
    case ShortcutRole:
        if (auto it = d->m_pendingShortcuts.constFind(action); it != d->m_pendingShortcuts.constEnd()) {
            return it.value();
        }
        return action->variantShortcut();
    case AlternateShortcutRole:
        if (auto it = d->m_pendingAlternateShortcuts.constFind(action); it != d->m_pendingAlternateShortcuts.constEnd()) {
            return it.value();
        }
        return action->variantAlternateShortcut();
    }

    return {};
}

bool ActionModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || (role != ShortcutRole && role != AlternateShortcutRole)) {
        return false;
    }
    ActionData *action = data(index, ActionDescriptionRole).value<ActionData *>();
    if (!action) {
        return false;
    }
    const QKeySequence seq = value.value<QKeySequence>();
    if (role == ShortcutRole) {
        d->m_pendingShortcuts[action] = seq;
    } else {
        d->m_pendingAlternateShortcuts[action] = seq;
    }
    Q_EMIT dataChanged(index, index, {role});
    return true;
}

Qt::ItemFlags ActionModel::flags(const QModelIndex &index) const
{
    auto f = QAbstractListModel::flags(index);

    if (index.isValid()) {
        f |= Qt::ItemIsEditable;
    }

    return f;
}

QHash<int, QByteArray> ActionModel::roleNames() const
{
    return {{Qt::DisplayRole, "display"},
            {ActionDescriptionRole, "actionDescription"},
            {ActionInstanceRole, "actionInstance"},
            {ActionCollectionRole, "actionCollection"},
            {ShortcutRole, "shortcut"},
            {AlternateShortcutRole, "alternateShortcut"}};
}

#include "moc_actionmodel.cpp"
