// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2025 Marco Martin <notmart@gmail.com>

#include "actioncollection.h"
#include "actioncollection_p.h"
#include "actioncollections.h"
#include "actioncollections_p.h"
#include "actiondata.h"
#include "standardactioncollection.h"

#include <KLocalizedString>
#include <KStandardShortcut>
#include <QCoreApplication>
#include <QKeySequence>
#include <QQmlContext>
#include <QtQml/qqmlinfo.h>

using namespace Qt::StringLiterals;

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

    return action;
}

QAction *ActionCollection::action(const QString &name)
{
    return d->m_actionMap.value(name);
}

QList<QAction *> ActionCollection::actions() const
{
    QList<QAction *> qActions;
    std::copy(d->m_actions.constBegin(), d->m_actions.constEnd(), std::back_inserter(qActions));
    return qActions;
}

#include "moc_actioncollection.cpp"
