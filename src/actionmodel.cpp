// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2025 Marco Martin <notmart@gmail.com>

#include "actionmodel.h"
#include "actioncollection.h"
#include "actiondata.h"

#include <QKeySequence>
#include <QPersistentModelIndex>
#include <QQmlContext>
#include <QQmlProperty>
#include <QtQml/qqmlinfo.h>

class ActionModelPrivate
{
public:
    ActionModelPrivate(ActionModel *model);
    void connectCollection(ActionCollection *collection);
    qsizetype collectionPosition(ActionCollection *collection) const;

    ActionModel *q;
    QString m_collectionName;
    QHash<ActionData *, QPair<QKeySequence, QKeySequence>> m_pendingChanges;
    ActionModel::ShownActions m_shownActions = ActionModel::AllActions;
};

ActionModelPrivate::ActionModelPrivate(ActionModel *model)
    : q(model)
{
}

void ActionModelPrivate::connectCollection(ActionCollection *collection)
{
    QObject::connect(collection, &ActionCollection::aboutToAddActionInstance, q, [this, collection](int position, QObject *action) {
        if (m_shownActions != ActionModel::ActiveActions) {
            return;
        }
        const int cp = collectionPosition(collection);
        q->beginInsertRows(QModelIndex(), cp + position, cp + position);
    });
    QObject::connect(collection, &ActionCollection::actionInstanceAdded, q, [this](int position, QObject *action) {
        if (m_shownActions != ActionModel::ActiveActions) {
            return;
        }
        q->endInsertRows();
    });
    QObject::connect(collection, &ActionCollection::aboutToRemoveActionInstance, q, [this](int position, QObject *action) {
        if (m_shownActions != ActionModel::ActiveActions) {
            return;
        }
        q->beginRemoveRows(QModelIndex(), position, position);
    });
    QObject::connect(collection, &ActionCollection::actionInstanceRemoved, q, [this](int position, QObject *action) {
        Q_UNUSED(position);
        Q_UNUSED(action);
        if (m_shownActions != ActionModel::ActiveActions) {
            return;
        }
        q->endRemoveRows();
    });
}

qsizetype ActionModelPrivate::collectionPosition(ActionCollection *collection) const
{
    qsizetype collectionPosition = 0;

    const QList<ActionCollection *> collections = ActionCollections::self()->collections();

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

////////////////////////////////////////

ActionModel::ActionModel(QObject *parent)
    : QAbstractListModel(parent)
    , d(new ActionModelPrivate(this))
{
    const QList<ActionCollection *> collections = ActionCollections::self()->collections();

    for (auto *coll : collections) {
        d->connectCollection(coll);
    }
    connect(ActionCollections::self(), &ActionCollections::collectionInserted, this, [this](ActionCollection *collection) {
        d->connectCollection(collection);
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
    for (auto it = d->m_pendingChanges.constBegin(); it != d->m_pendingChanges.constEnd(); ++it) {
        if (!it.value().first.isEmpty()) {
            it.key()->setVariantShortcut(it.value().first);
        }
    }
    d->m_pendingChanges.clear();
}

void ActionModel::reset(const QModelIndex &index)
{
    ActionData *action = index.data(ActionDescriptionRole).value<ActionData *>();
    if (!action) {
        return;
    }
    d->m_pendingChanges.remove(action);
    Q_EMIT dataChanged(index, index, {ShortcutRole, AlternateShortcutRole});
}

void ActionModel::resetAll()
{
    beginResetModel();
    d->m_pendingChanges.clear();
    endResetModel();
}

int ActionModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    const QList<ActionCollection *> collections = ActionCollections::self()->collections();

    if (d->m_shownActions == ActiveActions) {
        return std::accumulate(collections.constBegin(), collections.constEnd(), 0, [](int sum, ActionCollection *coll) {
            return sum + coll->actions().count();
        });
    }

    return std::accumulate(collections.constBegin(), collections.constEnd(), 0, [](int sum, ActionCollection *coll) {
        return sum + coll->actions().count();
    });
}

QVariant ActionModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(checkIndex(index, QAbstractItemModel::CheckIndexOption::IndexIsValid));

    qsizetype actualRow = index.row();
    ActionCollection *collection = nullptr;
    ActionData *action;
    const QList<ActionCollection *> collections = ActionCollections::self()->collections();
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
        if (d->m_pendingChanges.contains(action)) {
            return d->m_pendingChanges[action].first;
        }
        return action->variantShortcut();
    }

    return {};
}

bool ActionModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || role != ShortcutRole) {
        return false;
    }
    ActionData *action = data(index, ActionDescriptionRole).value<ActionData *>();
    if (!action) {
        return false;
    }
    const QKeySequence seq = value.value<QKeySequence>();
    d->m_pendingChanges[action].first = seq;
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
            {ShortcutRole, "shortcut"}};
}

#include "moc_actionmodel.cpp"
