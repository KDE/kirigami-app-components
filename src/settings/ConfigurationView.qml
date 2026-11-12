// SPDX-FileCopyrightText: 2024 Carl Schwan <carl@carlschwan.eu>
// SPDX-FileCopyrightText: 2026 Marco Martin <mart@kde.org>
// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

import QtQuick
import QtQml
import org.kde.ki18n
import org.kde.kirigami as Kirigami

/*!
   \qmltype ConfigurationView
   \inqmlmodule org.kde.kirigami.settings
   \brief This is an abstract view to display the configuration of an application.

   The various configuration modules can be defined by providing ConfigurationModule
   to the modules property.

   On desktop, this will display the modules in a list view and displaying
   the actual page next to it. On mobile, only the list of modules will be
   initially displayed.

   \qml
   import QtQuick.Controls as Controls
   import org.kde.kirigami.settings as KirigamiSettings

   Controls.Button {
       id: button

       KirigamiSettings.ConfigurationView {
           id: configuration

           window: button.Controls.ApplicationWindow.window as Kirigami.ApplicationWindow

           modules: [
               KirigamiSettings.ConfigurationModule {
                   moduleId: "appearance"
                   text: i18nc("@action:button", "Appearance")
                   icon.name: "preferences-desktop-theme-global"
                   page: () => Qt.createComponent("org.kde.tokodon", "AppearancePage")
               },
               ...
               KirigamiSettings.ConfigurationModule {
                   moduleId: "about"
                   text: i18nc("@action:button", "About Tokodon")
                   icon.name: "help-about"
                   page: () => Qt.createComponent("org.kde.kirigami", "AboutPage")
                   category: i18nc("@title:group", "About")
                   searchable: false
               },
               KirigamiSettings.ConfigurationModule {
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
    property list<ConfigurationModule> modules

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
       By default it is not specified, this will choose the first module.
     */
    function open(defaultModule = ''): void {
        if (!root.window) {
            console.warn("ConfigurationView: the window property must be set before calling open()");
            return;
        }

        if (root.configViewItem) {
            if (typeof root.configViewItem.requestActivate === "function") {
                Qt.callLater(root.configViewItem.requestActivate);
            }
            return;
        }

        let view = null;
        if (Kirigami.Settings.isMobile) {
            const component = Qt.createComponent('org.kde.kirigami.settings.private', 'ConfigMobilePage');
            if (component.status === Component.Error) {
                console.error(component.errorString());
                return;
            }
            view = root.window.pageStack.layers.push(component, {
                defaultModule: defaultModule,
                modules: root.modules,
                title: root.title,
                window: root.window,
            });
            if (view) {
                view.backRequested.connect(event => {
                    if (!event.accepted) {
                        view.destroy();
                    }
                });
            }
        } else {
            const component = Qt.createComponent('org.kde.kirigami.settings.private', 'ConfigWindow');
            if (component.status === Component.Error) {
                console.error(component.errorString());
                return;
            }
            view = component.createObject(root.window, {
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
                    }
                });
            }
        }

        if (!view) {
            console.error("ConfigurationView: failed to open the settings view");
            return;
        }

        // The view can also be closed and destroyed by other means (for
        // example by the page stack); make sure a subsequent open() works.
        view.destroyed.connect(() => root.configViewItem = null);
        root.configViewItem = view;
    }
}

