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

    AC.ActionCollection {
        name: "org.kde.collection"
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
        name: "org.kde.collection2"
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

    AC.StandardActionCollection {

    }

    Kirigami.ScrollablePage {
        id: testPage
        actions: [
            Kirigami.Action {
                objectName: "hello"
                AC.ActionCollection.collection: "org.kde.collection"
                onTriggered: {
                    print("hello")
                }
            },
            Kirigami.Action {
                objectName: "copy"
                AC.ActionCollection.collection: "org.kde.collection"
                onTriggered: {
                    print("copy")
                }
            },
            Kirigami.Action {
                objectName: "explore-actions"
                AC.ActionCollection.collection: "org.kde.collection"
                onTriggered: {
                    print("explore")
                    actionsDialog.open()
                }
            },
            Kirigami.Action {
                objectName: "configure-shortcuts"
                AC.ActionCollection.collection: "org.kde.collection"
                onTriggered: {
                    print("configure")
                    root.pageStack.push(shortcutsEditor)
                }
            },
            Kirigami.Action {
                objectName: "lorem"
                AC.ActionCollection.collection: "org.kde.collection2"
                onTriggered: {
                    onTriggered: print("Lorem Ipsum triggered")
                }
            },
            Kirigami.Action {
                objectName: "settings"
                AC.ActionCollection.collection: "org.kde.standardactions"
                onTriggered: {
                    print(this, text)
                    onTriggered: print("Settings triggered")
                }
            }
        ]

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
                collectionName: "org.kde.collection"
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
