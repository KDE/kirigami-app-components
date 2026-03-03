// SPDX-FileCopyrightText: 2025 Marco Martin <mart@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later

import QtQuick
import QtQuick.Controls as QQC
import QtQuick.Layouts

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
            pageRow.pushDialogLayer(shortcutsEditor)
        }
    }
    Connections {
        target: standardActions.action("FindAction")
        function onTriggered() {
            let dialog = actionsDialog.createObject(pageRow.parent);
            dialog.open()
        }
    }

    Component {
        id: shortcutsEditor
        ShortcutsEditor {}
    }

    Component {
        id: actionsDialog
        KQuickCommandBarPage {
            onClosed: {
                this.destroy();
            }
        }
    }
}
