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
    }

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
/*
    AC.StandardActionCollection {
        id: standardActions
    }*/



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
               /* onTriggered: {
                    root.pageStack.pushDialogLayer(shortcutsEditor)
                }*/
            },
            Kirigami.Action {
                displayHint: Kirigami.DisplayHint.AlwaysHide
                objectName: "FindAction"
                AC.ActionCollection.collection: "org.kde.standardactions"
             /*   onTriggered: {
                    actionsDialog.open()
                }*/
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
        /*[
            Kirigami.Action {
                objectName: "hello"
                AC.ActionCollection.collection: "org.kde.examplecollection"
                onTriggered: {
                    print("hello")
                }
            },
            Kirigami.Action {
                objectName: "copy"
                AC.ActionCollection.collection: "org.kde.examplecollection"
                onTriggered: {
                    print("copy")
                }
            },
            Kirigami.Action {
                objectName: "explore-actions"
                AC.ActionCollection.collection: "org.kde.examplecollection"
                onTriggered: {
                    print("explore")
                    actionsDialog.open()
                }
            },
            Kirigami.Action {
                objectName: "configure-shortcuts"
                AC.ActionCollection.collection: "org.kde.examplecollection"
                onTriggered: {
                    print("configure")
                    root.pageStack.push(shortcutsEditor)
                }
            },
            Kirigami.Action {
                objectName: "lorem"
                AC.ActionCollection.collection: "org.kde.examplecollection2"
                onTriggered: {
                    onTriggered: print("Lorem Ipsum triggered")
                }
            },
            Kirigami.Action {
                objectName: "Preferences"
                AC.ActionCollection.collection: "org.kde.standardactions"
                onTriggered: {
                    print(this, text)
                    onTriggered: print("Settings triggered")
                }
            }
        ]*/

        AC.KQuickCommandBarPage {
            id: actionsDialog
        }

        Component {
            id: shortcutsEditor
            AC.ShortcutsEditor {
            }
        }

      /*  ListView {
            model: AC.ActionModel {
                collectionName: "org.kde.examplecollection"
                shownActions: AC.ActionsModel.AllActions
            }
            delegate: RowLayout {
                required property string display
                required property AC.ActionData actionDescription
                Kirigami.Icon {
                    source: actionDescription.icon
                }
                QQC.Label {
                    text: display
                }
                QQC.Label {
                    text: actionDescription.defaultShortcut
                }
                KQ.KeySequenceItem {
                    keySequence: actionDescription.shortcut
                    onKeySequenceModified: {
                        if (keySequence == "") {
                            keySequence = actionDescription.defaultShortcut
                        }
                        actionDescription.shortcut = keySequence
                    }
                }
            }
        }*/
    }
}
