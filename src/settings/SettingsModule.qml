// SPDX-FileCopyrightText: 2021 Carl Schwan <carl\carlschwan.eu>
// SPDX-FileCopyrightText: 2026 Marco Martin <mart@kde.org>
// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

import QtQml
import org.kde.kirigami as Kirigami

/*!
   \qmltype SettingsModule
   \inqmlmodule org.kde.kirigami.settings
   \brief This object holds the information of configuration module.
 */
QtObject {
    id: root

    /*!
       \brief This property holds the id of the module for when it needs to be referenced.

       Primary use case is for setting a default module in SettingsViewController.
     */
    required property string moduleId

    /*!
       This property holds the name of the module.

       This will be displayed in the list of modules.
     */
    required property string name

    /*!
       This property holds the icon of the module.
     */
    readonly property Kirigami.IconPropertiesGroup icon: Kirigami.IconPropertiesGroup {}

    /*!
       This property holds the page for this module.
     */
    required property Component page

    /*!
       This property holds whether the module is visible.
       \default true
     */
    property bool visible: true

    /*!
       Additional search terms for this module.

       Keywords are matched in addition to the module text, and are also used
       when \c searchable is false.

       \qml
       keywords: [
           i18nc("@info:search keyword", "Hotkey"),
           i18nc("@info:search keyword", "Key binding")
       ]
       \endqml
     */
    property list<string> keywords

    /*!
       This property holds the initial property that the page needs to be initialized with
       if any.

       This is a function that returns an Javascript object.

       \qml
       initialProperties: () => {
           return {
               room: root._room,
               connection: root._connection
           };
       }
       \endqml
     */
    property var initialProperties: () => {
        return {};
    }

    /*!
       This property holds the category of the module.

       This is used to group modules together on mobile.

       Do not overwrite the default value if you want your module to
       be grouped in the default category.
       \default ""
     */
    property string category: ""
}
