// SPDX-FileCopyrightText: 2025 Marco Martin <mart@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later

import QtQuick
import QtQuick.Controls as QQC
import QtQuick.Layouts

import org.kde.coreaddons as CoreAddons
import org.kde.kirigami as Kirigami
import org.kde.kirigami.actioncollection as AC

Item {
    id: root

    readonly property alias standardCollection: standardActions
    //FIXME: shouldn't require the app being based on pagerow
    required property Kirigami.PageRow pageRow

    default property list<AC.ActionCollection> collections

    AC.ActionCollection {
        id: standardActions
        name: "org.kde.globalactions"
        text: CoreAddons.AboutData.displayName
        AC.StandardActionData {
            standardAction: AC.StandardActionData.Preferences
            text: i18ndc("kirigami-actioncollection", "Configure application menu entry", "Configure %1…", CoreAddons.AboutData.displayName)
        }
        AC.StandardActionData {
            standardAction: AC.StandardActionData.KeyBindings
            onTriggered: {
                root.pageRow.pushDialogLayer(shortcutsEditor)
            }
        }
        AC.ActionData {
            name: "FindAction"
            text: i18nc("Opens the actions search dialog", "Find Action…")
            icon.name: "search"
            defaultShortcut: "Ctrl+Alt+I"
            onTriggered: {
                let dialog = actionsDialog.createObject(root.pageRow.parent);
                dialog.open()
            }
        }
        AC.StandardActionData {
            standardAction: AC.StandardActionData.ReportBug
            onTriggered: {
                Qt.openUrlExternally("https://bugs.kde.org/enter_bug.cgi?format=guided&product=" + Qt.application.name + "&version=" + CoreAddons.AboutData.version)
            }
        }
        AC.StandardActionData {
            standardAction: AC.StandardActionData.Donate
            onTriggered: {
                Qt.openUrlExternally("https://kde.org/donate/?app=" + Qt.application.name)
            }
        }
        AC.StandardActionData {
            standardAction: AC.StandardActionData.AboutApp
            onTriggered: {
                // TODO: port to new forms
                const openDialogWindow = pageStack.pushDialogLayer(Qt.createComponent("org.kde.kirigamiaddons.formcard", "AboutPage"), {
                    width: root.width
                }, {
                    width: Kirigami.Units.gridUnit * 30,
                    height: Kirigami.Units.gridUnit * 30,
                    title: i18ndc("kirigami-actioncollection", "@title:window", "About %1", CoreAddons.AboutData.displayName),
                });
                openDialogWindow.Keys.escapePressed.connect(function() {
                    openDialogWindow.closeDialog();
                });
            }
        }
        AC.StandardActionData {
            standardAction: AC.StandardActionData.AboutKDE
            onTriggered: {
                const openDialogWindow = pageStack.pushDialogLayer(Qt.createComponent("org.kde.kirigamiaddons.formcard", "AboutKDEPage"), {
                    width: root.width
                }, {
                    width: Kirigami.Units.gridUnit * 30,
                    height: Kirigami.Units.gridUnit * 30,
                    title: i18ndc("kirigami-actioncollection", "@title:window", "About KDE"),
                });
                openDialogWindow.Keys.escapePressed.connect(function() {
                    openDialogWindow.closeDialog();
                });
            }
        }
    }

    Component {
        id: shortcutsEditor
        ShortcutsEditor {}
    }

    Component {
        id: actionsDialog
        AC.ActionsExplorer {
            onClosed: {
                this.destroy();
            }
        }
    }
}
