// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2025 Marco Martin <notmart@gmail.com>

#include "actioncollection.h"
#include "actioncollection_p.h"
#include "actioncollections.h"
#include "actioncollections_p.h"
#include "actiondata_p.h"

#include <KLocalizedString>
#include <KStandardActions>
#include <KStandardShortcut>
#include <QCoreApplication>
#include <QKeySequence>
#include <QQmlContext>
#include <QtQml/qqmlinfo.h>

#include <kstandardactions_p.h>

using namespace Qt::StringLiterals;
using namespace KirigamiActions;

ActionCollectionPrivate::ActionCollectionPrivate()
{
}

ActionCollectionPrivate::~ActionCollectionPrivate()
{
}

ActionCollection::ActionCollection(QObject *parent)
    : QObject(parent)
    , d(std::make_unique<ActionCollectionPrivate>())
{
}

ActionCollection::~ActionCollection()
{
}

QString ActionCollection::name() const
{
    return d->m_name;
}

QString ActionCollection::text() const
{
    if (d->m_text.isEmpty()) {
        return d->m_name;
    }

    return d->m_text;
}

void ActionCollection::setText(const QString &text)
{
    if (text == d->m_text) {
        return;
    }

    d->m_text = text;

    Q_EMIT textChanged(text);
}

QAction *ActionCollection::createAction(const QString &name, const QString &iconName, const QString &text)
{
    if (d->m_actionMap.contains(name)) {
        return nullptr;
    }

    auto action = new ActionData(this);
    action->setName(name);
    action->icon()->setName(iconName);
    action->setText(text);
    d->m_actionMap[name] = action;
    d->m_actions.append(action);
    Q_EMIT actionInserted(action);

    return action;
}

QAction *ActionCollection::createAction(KStandardActions::StandardAction standardAction)
{
    const KStandardActions::KStandardActionsInfo *info = infoPtr(standardAction);
    if (!info) {
        return nullptr;
    }

    const QString name = info->psName.toString();
    if (d->m_actionMap.contains(info->psName.toString())) {
        return nullptr;
    }

    auto action = new ActionData(this);
    action->classBegin();
    action->setName(name);
    action->icon()->setName(info->psIconName.toString());
    action->setText(QCoreApplication::translate("KStandardActions", info->psLabel));
    action->setToolTip(QCoreApplication::translate("KStandardActions", info->psToolTip));
    action->setDefaultShortcut(KStandardShortcut::shortcut(info->idAccel).first());
    action->componentComplete();
    d->m_actionMap[name] = action;
    d->m_actions.append(action);
    Q_EMIT actionInserted(action);
    return action;
}

QAction *ActionCollection::action(const QString &name)
{
    return d->m_actionMap.value(name);
}

QAction *ActionCollection::action(KStandardActions::StandardAction standardAction)
{
    const KStandardActions::KStandardActionsInfo *info = infoPtr(standardAction);
    if (!info) {
        return nullptr;
    }

    return d->m_actionMap.value(info->psName.toString());
}

QList<QAction *> ActionCollection::actions() const
{
    QList<QAction *> qActions;
    std::copy(d->m_actions.constBegin(), d->m_actions.constEnd(), std::back_inserter(qActions));
    return qActions;
}

#include "moc_actioncollection.cpp"
