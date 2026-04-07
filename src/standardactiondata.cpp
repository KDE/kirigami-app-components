// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2025 Marco Martin <notmart@gmail.com>

#include "qmlactioncollection_p.h"
#include "standardactiondata_p.h"

#include "actioncollection_p.h"

#include <KLocalizedString>

#include <kstandardactions_p.h>

StandardActionData::StandardActionData(QObject *parent)
    : ActionData(parent)
{
}

StandardActionData::~StandardActionData()
{
}

StandardActionData::StandardAction StandardActionData::standardAction() const
{
    return m_standardAction;
}

void StandardActionData::setStandardAction(StandardActionData::StandardAction standardAction)
{
    if (m_standardAction == standardAction) {
        return;
    }

    m_standardAction = standardAction;

    const KStandardActions::KStandardActionsInfo *info = infoPtr(KStandardActions::StandardAction(standardAction));
    if (!info) {
        return;
    }

    setName(info->psName.toString());

    if (standardAction == AboutApp) {
        icon()->setName(QGuiApplication::windowIcon().name().isEmpty() ? u"help-about"_s : QGuiApplication::windowIcon().name());
        setText(i18nc("About [application name]", "About %1…", QGuiApplication::applicationDisplayName()));
    } else if (standardAction == Donate) {
        const QString currencyCode = QLocale().currencySymbol(QLocale::CurrencyIsoCode).toLower();
        icon()->setName(QStringLiteral("help-donate-%1").arg(currencyCode));
        setText(QCoreApplication::translate("KStandardActions", info->psLabel));
    } else {
        icon()->setName(info->psIconName.toString());
        setText(QCoreApplication::translate("KStandardActions", info->psLabel));
    }

    setToolTip(QCoreApplication::translate("KStandardActions", info->psToolTip));
    const auto shortcuts = KStandardShortcut::shortcut(info->idAccel);
    if (!shortcuts.isEmpty()) {
        setDefaultShortcut(shortcuts.first());
    }

    Q_EMIT standardActionChanged(standardAction);
}

#include "moc_standardactiondata_p.cpp"
