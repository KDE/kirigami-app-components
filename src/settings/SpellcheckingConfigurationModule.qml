// SPDX-FileCopyrightText: 2024 Carl Schwan <carl@carlschwan.eu>
// SPDX-FileCopyrightText: 2026 Marco Martin <mart@kde.org>
// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

import QtQml
import org.kde.ki18n

/*!
   \qmltype SpellcheckingConfigurationModule
   \inqmlmodule org.kde.kirigami.settings
   \brief Configuration module for spellchecking.
 */
ConfigurationModule {
    moduleId: "spellchecking"
    text: KI18n.i18ndc("kirigami-app-components", "@action:button", "Spell Checking")
    icon.name: "tools-check-spelling"
    page: () => Qt.createComponent("org.kde.kirigami.settings.private", "SonnetConfigPage")
    visible: Qt.platform.os !== "android"
    keywords: [
        KI18n.i18ndc("kirigami-app-components", "@info:search keyword", "Spelling"),
        KI18n.i18ndc("kirigami-app-components", "@info:search keyword", "Dictionary")
    ]
}
