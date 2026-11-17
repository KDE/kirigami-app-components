// SPDX-FileCopyrightText: 2024 Carl Schwan <carl@carlschwan.eu>
// SPDX-FileCopyrightText: 2026 Marco Martin <mart@kde.org>
// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

import QtQml
import org.kde.ki18n
import org.kde.kirigami as Kirigami

/*!
   \qmltype ShortcutsSettingsModule
   \inqmlmodule org.kde.kirigami.settings
   \brief Configuration module for keyboard shortcuts.
 */
SettingsModule {
    id: root
    moduleId: "shortcuts"

    /*!
       \brief This property holds whether we allow shortcuts without a modifier (i.e. just "T" instead of "Ctrl+T")

       \default false
     */
    property bool allowModifierlessShortcuts: false

    name: _tr.i18nd("@action:button", "Keyboard Shortcuts")
    icon.name: "configure-shortcuts-symbolic"
    visible: !Kirigami.Settings.isMobile
    page: Qt.createComponent("org.kde.kirigami.actioncollection", "ShortcutsEditor")
    initialProperties: () => ({ allowModifierlessShortcuts: root.allowModifierlessShortcuts })
    keywords: [
        _tr.i18nd("@info:search keyword", "Hotkey"),
        _tr.i18nd("@info:search keyword", "Key binding"),
        _tr.i18nd("@info:search keyword", "Accelerator")
    ]

    property KI18nContext _tr: KI18nContext {
        translationDomain: "kirigami-actioncollection"
    }
}
