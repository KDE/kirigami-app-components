// SPDX-License-Identifier: LGPL-2.1-or-later
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
            text: i18n("Example Collection")
            AC.ActionData {
                name: "hello"
                text: "Hello"
                icon.name: "document-send"
                defaultShortcut: "Ctrl+H"
            }
            AC.StandardActionData {
                standardAction: AC.StandardActionData.Copy
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
            text: i18n("Other Actions")
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
                AC.ActionCollection.collection: "org.kde.examplecollection"
                AC.ActionCollection.action: "hello"
                onTriggered: {
                    print("hello triggered")
                }
            },
            Kirigami.Action {
                AC.ActionCollection.collection: "org.kde.examplecollection"
                AC.ActionCollection.action: "edit_copy"
                onTriggered: {
                    print("copy triggered")
                }
            },
            Kirigami.Action {
                AC.ActionCollection.collection: "org.kde.globalactions"
                AC.ActionCollection.action: "Preferences"
                displayHint: Kirigami.DisplayHint.AlwaysHide
                onTriggered: {
                    print("configure triggered")
                }
            },
            Kirigami.Action {
                AC.ActionCollection.collection: "org.kde.globalactions"
                AC.ActionCollection.action: "KeyBindings"
                displayHint: Kirigami.DisplayHint.AlwaysHide
            },
            Kirigami.Action {
                AC.ActionCollection.collection: "org.kde.globalactions"
                AC.ActionCollection.action: "FindAction"
                displayHint: Kirigami.DisplayHint.AlwaysHide
            },
            Kirigami.Action {
                displayHint: Kirigami.DisplayHint.AlwaysHide
                separator: true
            },
            Kirigami.Action {
                AC.ActionCollection.collection: "org.kde.globalactions"
                AC.ActionCollection.action: "ReportBug"
                displayHint: Kirigami.DisplayHint.AlwaysHide
            },
            Kirigami.Action {
                AC.ActionCollection.collection: "org.kde.globalactions"
                AC.ActionCollection.action: "Donate"
                displayHint: Kirigami.DisplayHint.AlwaysHide
            },
            Kirigami.Action {
                AC.ActionCollection.collection: "org.kde.globalactions"
                AC.ActionCollection.action: "AboutApp"
                displayHint: Kirigami.DisplayHint.AlwaysHide
            },
            Kirigami.Action {
                AC.ActionCollection.collection: "org.kde.globalactions"
                AC.ActionCollection.action: "AboutKDE"
                displayHint: Kirigami.DisplayHint.AlwaysHide
            }
        ]
    }
}
