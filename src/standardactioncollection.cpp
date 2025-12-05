// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2025 Marco Martin <notmart@gmail.com>

#include "standardactioncollection.h"
#include "actiondata.h"

#include <KLocalizedString>
#include <KStandardShortcut>
#include <QCoreApplication>
#include <QKeySequence>

StandardActionCollection::StandardActionCollection(QObject *parent)
    : ActionCollection(parent)
{
    setName(QStringLiteral("org.kde.standardactions"));
    ActionData *a = new ActionData();
    a->setName(QStringLiteral("settings"));
    a->icon()->setName(QStringLiteral("settings-configure"));
    a->setText(i18nc("Configure [application name]", "Configure %1…", QCoreApplication::applicationName()));
    auto shortcuts = KStandardShortcut::shortcut(KStandardShortcut::Preferences);
    if (!shortcuts.isEmpty()) {
        a->setShortcut(shortcuts.first());
    }
    shortcuts = KStandardShortcut::hardcodedDefaultShortcut(KStandardShortcut::Preferences);
    if (!shortcuts.isEmpty()) {
        a->setDefaultShortcut(shortcuts.first());
    }
    insertAction(a);
}

StandardActionCollection::~StandardActionCollection()
{
}

#include "moc_standardactioncollection.cpp"
