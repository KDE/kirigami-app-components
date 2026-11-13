// SPDX-FileCopyrightText: 2024 Carl Schwan <carl@carlschwan.eu>
// SPDX-FileCopyrightText: 2026 Marco Martin <mart@kde.org>
// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

import QtQml
import org.kde.ki18n
import org.kde.kirigami as Kirigami
import org.kde.kirigami.settings.private as Private

/*!
   \qmltype SettingsViewController
   \inqmlmodule org.kde.kirigami.settings
   \brief Controls the views that display the configuration of an application.

   The various configuration modules can be defined by providing SettingsModule
   to the modules property.

   On desktop, this will display the modules in a list view and displaying
   the actual page next to it. On mobile, only the list of modules will be
   initially displayed.

   \qml
   import QtQuick.Controls as Controls
   import org.kde.kirigami.settings as KirigamiSettings

   Controls.Button {
       id: button

       KirigamiSettings.SettingsViewController {
           id: configuration

           window: button.Controls.ApplicationWindow.window as Kirigami.ApplicationWindow

           modules: [
               KirigamiSettings.SettingsModule {
                   moduleId: "appearance"
                   text: i18nc("@action:button", "Appearance")
                   icon.name: "preferences-desktop-theme-global"
                   page: () => Qt.createComponent("org.kde.tokodon", "AppearancePage")
               },
               ...
               KirigamiSettings.SettingsModule {
                   moduleId: "about"
                   text: i18nc("@action:button", "About Tokodon")
                   icon.name: "help-about"
                   page: () => Qt.createComponent("org.kde.kirigami", "AboutPage")
                   category: i18nc("@title:group", "About")
                   searchable: false
               },
               KirigamiSettings.SettingsModule {
                   moduleId: "aboutkde"
                   text: i18nc("@action:button", "About KDE")
                   icon.name: "kde"
                   page: () => Qt.createComponent("org.kde.kirigami.actioncollection", "AboutKDEPage")
                   category: i18nc("@title:group", "About")
                   searchable: false
               }
           ]
       }

       icon.name: 'settings-configure-symbolic'
       text: i18nc("@action:button", "Settings")

       onClicked: configuration.open()
   }
   \endqml

   This will result in the following dialog on desktop:

   \image settingsdialogdesktop.png

   And the following page on mobile:

   \image settingsdialogmobile.png
 */
QtObject {
    id: root

    /*!
       \brief This property holds the title of the config view.

       \default "Settings"
     */
    property string title: KI18n.i18ndc("kirigami-app-components", "@title:window", "Settings")

    /*!
       \brief This property holds the list of pages for the settings.
     */
    property list<SettingsModule> modules

    /*!
       \brief This property holds the parent window.

       This needs to be set before calling open.
     */
    property Kirigami.ApplicationWindow window

    /*!
       \brief The current config page/window depending on platform.

       Null if one currently doesn't exist.
     */
    property QtObject configViewItem: null

    /*!
       Open the configuration window for the given \a defaultModule.

       The defaultModule corresponds to the moduleId of the default configuration that should be preselected when opening the configuration view.
       By default it is not specified, on desktop the first module will be chosen, on mobile no page will be preselected.
     */
    function open(defaultModule = ''): void {
        if (!root.window) {
            console.warn("SettingsViewController: the window property must be set before calling open()");
            return;
        }

        if (root.configViewItem) {
            if (typeof root.configViewItem.objectName !== "string") {
                // The previous view was destroyed by other means (for
                // example by the page stack); open a new one.
                root.configViewItem = null;
            } else if (typeof root.configViewItem.requestActivate === "function") {
                Qt.callLater(root.configViewItem.requestActivate);
                return;
            } else {
                return;
            }
        }

        let view = null;
        if (Kirigami.Settings.isMobile) {
            view = root.window.pageStack.layers.push(d.mobilePageComponent, {
                defaultModule: defaultModule,
                modules: root.modules,
                title: root.title,
                window: root.window,
            });
            if (view) {
                view.backRequested.connect(event => {
                    if (!event.accepted) {
                        view.destroy();
                        root.configViewItem = null;
                    }
                });
            }
        } else {
            view = d.windowComponent.createObject(root.window, {
                defaultModule: defaultModule,
                modules: root.modules,
                width: Kirigami.Units.gridUnit * 50,
                height: Kirigami.Units.gridUnit * 30,
                minimumWidth: Kirigami.Units.gridUnit * 50,
                minimumHeight: Kirigami.Units.gridUnit * 30,
                title: root.title,
            });
            if (view) {
                view.closing.connect(event => {
                    if (event.accepted) {
                        view.destroy();
                        root.configViewItem = null;
                    }
                });
            }
        }

        if (!view) {
            console.error("SettingsViewController: failed to open the settings view");
            return;
        }

        root.configViewItem = view;
    }

    readonly property QtObject __d: QtObject {
        id: d
        // Mobile
        readonly property Component mobilePageComponent: Private.ConfigMobilePage {}
        // Desktop
        readonly property Component windowComponent: Private.ConfigWindow {}
    }
}

