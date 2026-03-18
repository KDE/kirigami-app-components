// SPDX-License-Identifier: LGPL-2.1-or-later
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
    case KStandardShortcut::KeyBindings:
        return u"configure-shortcuts"_s;
    case KStandardShortcut::ReportBug:
        return u"tools-report-bug"_s;
    case KStandardShortcut::Donate: {
        const QString currencyCode = QLocale().currencySymbol(QLocale::CurrencyIsoCode).toLower();
        if (!currencyCode.isEmpty()) {
            return QStringLiteral("help-donate-%1").arg(currencyCode);
        }
        return u"help-donate"_s;
    }
    case KStandardShortcut::AboutApp:
        return QGuiApplication::windowIcon().name().isEmpty() ? u"help-about"_s : QGuiApplication::windowIcon().name();
    case KStandardShortcut::AboutKDE:
        return u"kde"_s;
    default:
        return {};
    }

    return {};
}

StandardActionCollection::StandardActionCollection(QObject *parent)
    : QmlActionCollection(parent)
{
    setName(u"org.kde.standardactions"_s);
    setText(QGuiApplication::applicationDisplayName());

    ActionData *a = new ActionData(this);
    a->classBegin();
    a->setName(u"FindAction"_s);
    a->setText(i18nc("Opens the actions search dialog", "Find Action…"));
    a->icon()->setName(u"search"_s);
    a->setDefaultShortcut(QKeySequence(Qt::CTRL | Qt::ALT | Qt::Key_I));
    insertAction(a);
    a->componentComplete();

    const QList<KStandardShortcut::StandardShortcut> standardShortcuts = {KStandardShortcut::Preferences,
                                                                          KStandardShortcut::KeyBindings,
                                                                          KStandardShortcut::ReportBug,
                                                                          KStandardShortcut::Donate,
                                                                          KStandardShortcut::AboutApp,
                                                                          KStandardShortcut::AboutKDE};
    for (const auto id : standardShortcuts) {
        ActionData *a = new ActionData(this);
        a->classBegin();
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
        a->componentComplete();
    }

    a = qobject_cast<ActionData *>(action(u"KeyBindings"_s));
    Q_ASSERT(a);
    a->setText(i18nc("Open keyboard shortcut config dialog", "Configure Keyboard Shortcuts…"));

    a = qobject_cast<ActionData *>(action(u"Preferences"_s));
    Q_ASSERT(a);
    a->setText(i18nc("Configure [application name]", "Configure %1…", QGuiApplication::applicationDisplayName()));

    a = qobject_cast<ActionData *>(action(u"AboutApp"_s));
    Q_ASSERT(a);
    a->setText(i18nc("About [application name]", "About %1…", QGuiApplication::applicationDisplayName()));
}

StandardActionCollection::~StandardActionCollection()
{
}

#include "moc_standardactioncollection.cpp"
