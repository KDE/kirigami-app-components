// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2026 Marco Martin <notmart@gmail.com>

#include "actioncollection_p.h"
#include "actioncollections_p.h"
#include "actiondata_p.h"
#include "qmlactioncollection_p.h"
#include "standardactiondata_p.h"

#include <kstandardactions_p.h>

#include <KLocalizedString>
#include <KStandardShortcut>
#include <QCoreApplication>
#include <QKeySequence>
#include <QQmlContext>
#include <QQmlProperty>
#include <QtQml/qqmlinfo.h>

using namespace Qt::StringLiterals;

ActionCollectionAttached::ActionCollectionAttached(QObject *parent)
    : QObject(parent)
{
}

ActionCollectionAttached::~ActionCollectionAttached()
{
}

QString ActionCollectionAttached::collection() const
{
    return m_collection;
}

void ActionCollectionAttached::setCollection(const QString &collection)
{
    if (m_collection == collection)
        return;

    m_collection = collection;

    QmlActionCollection *coll = qobject_cast<QmlActionCollection *>(ActionCollections::self()->collection(m_collection));
    if (coll) {
        connect(coll, &ActionCollection::actionInserted, this, [this]() {
            rebindActionData();
        });
    }

    rebindActionData();

    Q_EMIT collectionChanged();
}

QVariant ActionCollectionAttached::action() const
{
    return m_actionName;
}

void ActionCollectionAttached::setAction(const QVariant &action)
{
    if (action.value<StandardActionData::StandardAction>() == StandardActionData::ActionNone) {
        m_actionName = action.toString();
    } else {
        StandardActionData::StandardAction standardAction = action.value<StandardActionData::StandardAction>();
        const KStandardActions::KStandardActionsInfo *info = infoPtr(KStandardActions::StandardAction(standardAction));
        if (info) {
            m_actionName = info->psName.toString();
        }
    }

    rebindActionData();

    Q_EMIT actionChanged();
}

void ActionCollectionAttached::rebindActionData()
{
    if (m_action) {
        m_action->setAction(nullptr);
        m_action.clear();
    }

    if (m_actionName.isEmpty()) {
        return;
    }

    QmlActionCollection *coll = qobject_cast<QmlActionCollection *>(ActionCollections::self()->collection(m_collection));
    if (!coll) {
        return;
    }

    m_action = qobject_cast<ActionData *>(coll->action(m_actionName));

    if (!m_action) {
        return;
    }

    m_action->addKirigamiAction(parent());
}

/////////////////////////////////////

QmlActionCollection::QmlActionCollection(QObject *parent)
    : ActionCollection(parent)
{
    connect(this, &QmlActionCollection::nameChanged, this, [this]() {
        ActionCollections::self()->d->insertCollection(this);
    });
}

QmlActionCollection::~QmlActionCollection()
{
}

void QmlActionCollection::setName(const QString &name)
{
    // Allow setting the name only once
    if (name == d->m_name || !d->m_name.isEmpty()) {
        return;
    }

    d->m_name = name;

    Q_EMIT nameChanged(name);
}

void QmlActionCollection::insertAction(ActionData *action)
{
    if (!action) {
        return;
    }
    const QString name = action->name();
    if (d->m_actionMap.contains(name)) {
        return;
    }

    d->m_actions.append(action);
    d->m_actionMap[name] = action;

    connect(action, &QObject::destroyed, this, [this, action, name]() {
        d->m_actionMap.remove(name);
        d->m_actions.removeAll(action);
    });

    if (!action->kirigamiActions().isEmpty()) {
        Q_ASSERT(!d->m_activeActions.contains(action));
        const int pos = d->m_activeActions.count();
        Q_EMIT aboutToAddActionInstance(pos, action);
        d->m_activeActions.append(action);
        Q_EMIT actionInstanceAdded(pos, action);
    }

    connect(action, &ActionData::actionChanged, this, [this, action](QObject *actionInstance) {
        if (actionInstance) {
            Q_ASSERT(!d->m_activeActions.contains(action));
            const int pos = d->m_activeActions.count();
            Q_EMIT aboutToAddActionInstance(pos, action);
            d->m_activeActions.append(action);
            Q_EMIT actionInstanceAdded(pos, action);
        } else {
            const int pos = d->m_activeActions.indexOf(action);
            Q_ASSERT(pos >= 0);
            Q_EMIT aboutToRemoveActionInstance(pos, action);
            d->m_activeActions.removeAll(action);
            Q_EMIT actionInstanceRemoved(pos, action);
        }
    });

    action->setParent(this);
}

QList<ActionData *> QmlActionCollection::activeActions() const
{
    return d->m_actions;
}

ActionCollectionAttached *QmlActionCollection::qmlAttachedProperties(QObject *object)
{
    return new ActionCollectionAttached(object);
}

void QmlActionCollection::actions_append(QQmlListProperty<ActionData> *prop, ActionData *action)
{
    // This can only be called from QML
    QmlActionCollection *coll = static_cast<QmlActionCollection *>(prop->object);
    if (!coll) {
        return;
    }

    const QString name = action->name();
    if (coll->d->m_actionMap.contains(name) && !name.isEmpty()) { // FIXME: name should be checked later
        QQmlError error;
        error.setDescription(QStringLiteral("ActionData name ") % name % QStringLiteral(" is not unique"));
        error.setMessageType(QtFatalMsg);
        QQmlContext *context = qmlContext(coll);
        if (context) {
            error.setUrl(context->baseUrl());
        }
        qmlWarning(nullptr, error);
        qFatal();
    }

    auto setupAction = [coll, action, name]() {
        coll->d->m_actions.append(action);
        coll->d->m_actionMap[action->name()] = action;

        connect(action, &QObject::destroyed, coll, [coll, action, name]() {
            coll->d->m_actionMap.remove(name);
            coll->d->m_actions.removeAll(action);
        });

        if (!action->kirigamiActions().isEmpty()) {
            Q_ASSERT(!coll->d->m_activeActions.contains(action));
            const int pos = coll->d->m_activeActions.count();
            Q_EMIT coll->aboutToAddActionInstance(pos, action);
            coll->d->m_activeActions.append(action);
            Q_EMIT coll->actionInstanceAdded(pos, action);
        }

        connect(action, &ActionData::actionChanged, coll, [coll, action](QObject *actionInstance) {
            if (actionInstance) {
                Q_ASSERT(!coll->d->m_activeActions.contains(action));
                const int pos = coll->d->m_activeActions.count();
                Q_EMIT coll->aboutToAddActionInstance(pos, action);
                coll->d->m_activeActions.append(action);
                Q_EMIT coll->actionInstanceAdded(pos, action);
            } else {
                const int pos = coll->d->m_activeActions.indexOf(action);
                Q_ASSERT(pos >= 0);
                Q_EMIT coll->aboutToRemoveActionInstance(pos, action);
                coll->d->m_activeActions.removeAll(action);
                Q_EMIT coll->actionInstanceRemoved(pos, action);
            }
        });

        Q_EMIT coll->actionInserted(action);
    };

    if (name.isEmpty()) {
        connect(action, &ActionData::nameChanged, action, setupAction, Qt::SingleShotConnection);
    } else {
        setupAction();
    }

    action->setParent(coll);
}

qsizetype QmlActionCollection::actions_count(QQmlListProperty<ActionData> *prop)
{
    QmlActionCollection *coll = static_cast<QmlActionCollection *>(prop->object);
    if (!coll) {
        return 0;
    }

    return coll->d->m_actions.count();
}

ActionData *QmlActionCollection::actions_at(QQmlListProperty<ActionData> *prop, qsizetype index)
{
    QmlActionCollection *coll = static_cast<QmlActionCollection *>(prop->object);
    if (!coll) {
        return nullptr;
    }

    if (index < 0 || index >= coll->d->m_actions.count()) {
        return nullptr;
    }
    return coll->d->m_actions.value(index);
}

void QmlActionCollection::actions_clear(QQmlListProperty<ActionData> *prop)
{
    QmlActionCollection *coll = static_cast<QmlActionCollection *>(prop->object);
    if (!coll) {
        return;
    }

    coll->d->m_actionMap.clear();
    coll->d->m_actions.clear();
}

QQmlListProperty<ActionData> QmlActionCollection::actionsProperty()
{
    return QQmlListProperty<ActionData>(this, nullptr, actions_append, actions_count, actions_at, actions_clear);
}

#include "moc_qmlactioncollection_p.cpp"
