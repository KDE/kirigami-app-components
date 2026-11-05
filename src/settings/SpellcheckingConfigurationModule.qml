// SPDX-FileCopyrightText: 2024 Carl Schwan <carl\carlschwan.eu>
// SPDX-FileCopyrightText: 2026 Marco Martin <mart@kde.org>
// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

import QtQuick
import org.kde.kirigamiaddons.private

/*!
   \qmltype SpellcheckingConfigurationModule
   \inqmlmodule org.kde.kirigami.settings
   \brief Configuration module for spellchecking.
   \since 1.3.0
 */
ConfigurationModule {
    moduleId: "spellchecking"
    text: I18n.i18nc("@action:button", "Spell Checking")
    icon.name: "tools-check-spelling"
    page: () => Qt.createComponent("org.kde.kirigami.settings.private", "SonnetConfigPage")
    visible: Qt.platform.os !== "android"
}
