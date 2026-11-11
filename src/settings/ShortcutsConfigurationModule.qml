// SPDX-FileCopyrightText: 2024 Carl Schwan <carl\carlschwan.eu>
// SPDX-FileCopyrightText: 2026 Marco Martin <mart@kde.org>
// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

import org.kde.ki18n
import org.kde.kirigami as Kirigami

/*!
   \qmltype ShortcutsConfigurationModule
   \inqmlmodule org.kde.kirigami.settings
   \brief Configuration module for keyboard shortcuts.
 */
ConfigurationModule {
    moduleId: "shortcuts"
    text: KI18n.i18nc("@action:button", "Keyboard Shortcuts")
    icon.name: "configure-shortcuts-symbolic"
    visible: !Kirigami.Settings.isMobile
    page: () => Qt.createComponent("org.kde.kirigami.actioncollection", "ShortcutsEditor")
    initialProperties: () => ({ allowModifierlessShortcuts: true })
    keywords: [
        KI18n.i18nc("@info:search keyword", "Hotkey"),
        KI18n.i18nc("@info:search keyword", "Key binding"),
        KI18n.i18nc("@info:search keyword", "Accelerator")
    ]
}
