// SPDX-License-Identifier: LGPL-2.0-or-later
// SPDX-FileCopyrightText: 2025 Marco Martin <notmart@gmail.com>

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC
import org.kde.kirigami as Kirigami
import org.kde.kirigami.actioncollection as AC
import org.kde.kquickcontrols as KQ
import org.kde.kitemmodels

Kirigami.ApplicationWindow {
    id: root
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")

    pageStack {
        initialPage: testPage
    }

    AC.ActionCollectionManager {
        id: manager
        pageRow: pageStack

        AC.ActionCollection {
            name: "org.kde.examplecollection"
            AC.ActionData {
                name: "hello"
                text: "Hello"
                icon.name: "document-send"
                defaultShortcut: "Ctrl+H"
            }
            AC.ActionData {
                name: "copy"
                text: "Copy"
                icon.name: "edit-copy"
                defaultShortcut: StandardKey.Copy
            }
            AC.ActionData {
                name: "explore-actions"
                text: "Actions"
                icon.name: "view-form-action"
                defaultShortcut: "Ctrl+I"
            }
            AC.ActionData {
                name: "configure-shortcuts"
                text: "Shortcuts"
                icon.name: "configure-shortcuts"
            }
        }

        AC.ActionCollection {
            name: "org.kde.examplecollection2"
            text: Qt.application.name
            AC.ActionData {
                name: "lorem"
                text: "Lorem Ipsum"
            }
            AC.ActionData {
                name: "dolor"
                text: "Dolor Sit Amet"
                defaultShortcut: "Ctrl+D"
            }
        }
    }


    Kirigami.ScrollablePage {
        id: testPage
        actions: [
            Kirigami.Action {
                objectName: "hello"
                AC.ActionCollection.collection: "org.kde.examplecollection"
                onTriggered: {
                    print("hello triggered")
                }
            },
            Kirigami.Action {
                objectName: "copy"
                AC.ActionCollection.collection: "org.kde.examplecollection"
                onTriggered: {
                    print("copy triggered")
                }
            },
            Kirigami.Action {
                displayHint: Kirigami.DisplayHint.AlwaysHide
                objectName: "Preferences"
                AC.ActionCollection.collection: "org.kde.standardactions"
                onTriggered: {
                    print("configure triggered")
                }
            },
            Kirigami.Action {
                displayHint: Kirigami.DisplayHint.AlwaysHide
                objectName: "KeyBindings"
                AC.ActionCollection.collection: "org.kde.standardactions"
            },
            Kirigami.Action {
                displayHint: Kirigami.DisplayHint.AlwaysHide
                objectName: "FindAction"
                AC.ActionCollection.collection: "org.kde.standardactions"
            },
            Kirigami.Action {
                displayHint: Kirigami.DisplayHint.AlwaysHide
                separator: true
            },
            Kirigami.Action {
                displayHint: Kirigami.DisplayHint.AlwaysHide
                objectName: "ReportBug"
                AC.ActionCollection.collection: "org.kde.standardactions"
                onTriggered: Qt.openUrlExternally("https://bugs.kde.org/enter_bug.cgi?format=guided&product=" + Qt.application.name + "&version=" + AboutData.version)
            },
            Kirigami.Action {
                displayHint: Kirigami.DisplayHint.AlwaysHide
                objectName: "Donate"
                AC.ActionCollection.collection: "org.kde.standardactions"
                onTriggered: Qt.openUrlExternally("https://kde.org/donate/?app=" + Qt.application.name)
            },
            Kirigami.Action {
                displayHint: Kirigami.DisplayHint.AlwaysHide
                objectName: "AboutApp"
                AC.ActionCollection.collection: "org.kde.standardactions"
                onTriggered: root.pageStack.pushDialogLayer(Qt.createComponent("org.kde.kirigamiaddons.formcard", "AboutPage"))
            },
            Kirigami.Action {
                displayHint: Kirigami.DisplayHint.AlwaysHide
                objectName: "AboutKDE"
                AC.ActionCollection.collection: "org.kde.standardactions"
                onTriggered: root.pageStack.pushDialogLayer(Qt.createComponent("org.kde.kirigamiaddons.formcard", "AboutKDEPage"))
            }
        ]
    }
}
