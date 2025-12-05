// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2025 Marco Martin <notmart@gmail.com>

#include "standardactioncollection.h"
#include "actiondata.h"

#include <KLocalizedString>
#include <KStandardShortcut>
#include <QGuiApplication>
#include <QKeySequence>

using namespace Qt::StringLiterals;

QString iconName(KStandardShortcut::StandardShortcut id)
{
    switch (id) {
    case KStandardShortcut::Preferences:
        return u"settings-configure"_s;
    case KStandardShortcut::ReportBug:
        return u"tools-report-bug"_s;
    case KStandardShortcut::Donate:
        return u"help-donate"_s;
    case KStandardShortcut::AboutApp:
        return QGuiApplication::windowIcon().name().isEmpty() ? u"help-about"_s : QGuiApplication::windowIcon().name();
    case KStandardShortcut::AboutKDE:
        return u"settings-configure"_s;
    default:
        return {};
    }

    return {};
}

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
        a->icon()->setName(iconName(id));
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
    act->setText(i18nc("Configure [application name]", "Configure %1…", QGuiApplication::applicationDisplayName()));

    act = action(QStringLiteral("AboutApp"));
    Q_ASSERT(act);
    act->setText(i18nc("About [application name]", "About %1…", QGuiApplication::applicationDisplayName()));
}

StandardActionCollection::~StandardActionCollection()
{
}

#include "moc_standardactioncollection.cpp"
