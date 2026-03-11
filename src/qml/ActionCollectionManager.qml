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

    AC.StandardActionCollection {
        id: standardActions
    }

    Connections {
        target: standardActions.action("KeyBindings")
        function onTriggered() {
            root.pageRow.pushDialogLayer(shortcutsEditor)
        }
    }
    Connections {
        target: standardActions.action("FindAction")
        function onTriggered() {
            let dialog = actionsDialog.createObject(root.pageRow.parent);
            dialog.open()
        }
    }
    Connections {
        target: standardActions.action("AboutApp")
        function onTriggered(): void {
            // TODO: port to new forms
            const openDialogWindow = pageStack.pushDialogLayer(Qt.createComponent("org.kde.kirigamiaddons.formcard", "AboutPage"), {
                width: root.width
            }, {
                width: Kirigami.Units.gridUnit * 30,
                height: Kirigami.Units.gridUnit * 30,
                title: i18ndc("kirigami-addons6", "@title:window", "About %1", CoreAddons.AboutData.displayName),
            });
            openDialogWindow.Keys.escapePressed.connect(function() {
                openDialogWindow.closeDialog();
            });
        }
    }

    Connections {
        target: standardActions.action("AboutKDE")
        function onTriggered(): void {
            const openDialogWindow = pageStack.pushDialogLayer(Qt.createComponent("org.kde.kirigamiaddons.formcard", "AboutKDEPage"), {
                width: root.width
            }, {
                width: Kirigami.Units.gridUnit * 30,
                height: Kirigami.Units.gridUnit * 30,
                title: i18ndc("kirigami-addons6", "@title:window", "About KDE"),
            });
            openDialogWindow.Keys.escapePressed.connect(function() {
                openDialogWindow.closeDialog();
            });
        }
    }

    Connections {
        target: standardActions.action("Donate")
        function onTriggered(): void {
            Qt.openUrlExternally("https://kde.org/donate/?app=" + Qt.application.name)
        }
    }

    Connections {
        target: standardActions.action("ReportBug")
        function onTriggered(): void {
            Qt.openUrlExternally("https://bugs.kde.org/enter_bug.cgi?format=guided&product=" + Qt.application.name + "&version=" + CoreAddons.AboutData.version)
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
