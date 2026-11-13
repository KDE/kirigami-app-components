// SPDX-FileCopyrightText: 2024 Carl Schwan <carl@carlschwan.eu>
// SPDX-FileCopyrightText: 2026 Marco Martin <mart@kde.org>
// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

import QtQml
import org.kde.ki18n
import org.kde.kirigami as Kirigami

/*!
   \qmltype ShortcutsConfigurationModule
   \inqmlmodule org.kde.kirigami.settings
   \brief Configuration module for keyboard shortcuts.
 */
ConfigurationModule {
    id: root
    moduleId: "shortcuts"

    /*!
       \brief This property holds whether we allow shortcuts without a modifier (i.e. just "T" instead of "Ctrl+T")

       \default false
     */
    property bool allowModifierlessShortcuts: false

    text: KI18n.i18ndc("kirigami-app-components", "@action:button", "Keyboard Shortcuts")
    icon.name: "configure-shortcuts-symbolic"
    visible: !Kirigami.Settings.isMobile
    page: () => Qt.createComponent("org.kde.kirigami.actioncollection", "ShortcutsEditor")
    initialProperties: () => ({ allowModifierlessShortcuts: root.allowModifierlessShortcuts })
    keywords: [
        KI18n.i18ndc("kirigami-app-components", "@info:search keyword", "Hotkey"),
        KI18n.i18ndc("kirigami-app-components", "@info:search keyword", "Key binding"),
        KI18n.i18ndc("kirigami-app-components", "@info:search keyword", "Accelerator")
    ]
}
