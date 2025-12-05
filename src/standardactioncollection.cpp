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

    const QList<KStandardShortcut::StandardShortcut> standardShortcuts = {KStandardShortcut::Preferences,
                                                                          KStandardShortcut::ReportBug,
                                                                          KStandardShortcut::Donate,
                                                                          KStandardShortcut::AboutApp,
                                                                          KStandardShortcut::AboutKDE};
    for (const auto id : standardShortcuts) {
        ActionData *a = new ActionData();
        a->setName(KStandardShortcut::name(id));
        a->icon()->setName(QStringLiteral("settings-configure"));
        a->setText(KStandardShortcut::label(id));
        auto shortcuts = KStandardShortcut::shortcut(id);
        if (!shortcuts.isEmpty()) {
            a->setShortcut(shortcuts.first());
        }
        shortcuts = KStandardShortcut::hardcodedDefaultShortcut(id);
        if (!shortcuts.isEmpty()) {
            a->setDefaultShortcut(shortcuts.first());
        }
        insertAction(a);
    }

    auto *act = action(QStringLiteral("Preferences"));
    Q_ASSERT(act);
    act->setText(i18nc("Configure [application name]", "Configure %1…", QCoreApplication::applicationName()));

    act = action(QStringLiteral("AboutApp"));
    Q_ASSERT(act);
    act->setText(i18nc("About [application name]", "About %1…", QCoreApplication::applicationName()));
}

StandardActionCollection::~StandardActionCollection()
{
}

#include "moc_standardactioncollection.cpp"
