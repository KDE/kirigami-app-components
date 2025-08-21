// SPDX-License-Identifier: LGPL-2.0-or-later
// SPDX-FileCopyrightText: 2025 Marco Martin <notmart@gmail.com>

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC
import org.kde.kirigami as Kirigami
import actioncollection as AC
import org.kde.kquickcontrols as KQ
import org.kde.kitemmodels

Kirigami.ApplicationWindow {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")

    pageStack {
        initialPage: testPage
    }

    AC.ActionCollection {
        name: "org.kde.collection"
        ActionData {
            name: "hello"
            text: "Hello"
            icon: "document-send"
            defaultShortcut: "Ctrl+H"
        }
        ActionData {
            name: "copy"
            text: "Copy"
            icon: "edit-copy"
            defaultShortcut: StandardKey.Copy
        }
        ActionData {
            name: "explore-actions"
            text: "Actions"
            icon: "view-form-action"
            defaultShortcut: "Ctrl+I"
        }
    }

    Kirigami.ScrollablePage {
        id: testPage
        actions: [
            Kirigami.Action {
                text: "hello"
                objectName: "hello"
                AC.ActionCollection.collection: "org.kde.collection"
                onTriggered: {
                    print("hello")
                }
            },
            Kirigami.Action {
                text: "Copy"
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
            }
        ]

        QQC.Dialog {
            id: actionsDialog
            onVisibleChanged: {
                if (!visible) {
                    actionSearchField.text = "";
                }
            }
            contentItem: ColumnLayout {
                implicitHeight: actionsList.contentHeight
                Kirigami.SearchField {
                    id: actionSearchField
                    Layout.fillWidth: true
                }
                QQC.ScrollView {
                    Layout.fillWidth: true
                    ListView {
                        id: actionsList
                        model: KSortFilterProxyModel {
                            sourceModel: AC.ActionsModel {
                                collection: "org.kde.collection"
                            }
                            sortRole: Qt.DisplayRole
                            sortCaseSensitivity: Qt.CaseInsensitive
                            filterRole: Qt.DisplayRole
                            filterString: actionSearchField.text
                            filterCaseSensitivity: Qt.CaseInsensitive
                        }
                        delegate: QQC.ItemDelegate {
                            width: actionsList.width
                            required property QtObject actionInstance
                            icon.name: actionInstance.icon.name
                            text: actionInstance.text + " " + actionInstance.shortcut
                            onClicked: {
                                actionInstance.trigger()
                                actionsDialog.close()
                            }
                        }
                    }
                }
            }
        }

        ListView {
            /*model: AC.ShortcutsModel {
                id: collectionModel
                name: "org.kde.collection"
            }*/
            delegate: RowLayout {
                Kirigami.Icon {
                    source: model.iconName
                }
                QQC.Label {
                    text: model.display
                }
                QQC.Label {
                    text: model.defaultShortcut
                }
                KQ.KeySequenceItem {
                    keySequence: model.shortcut
                    onKeySequenceModified: {
                        if (keySequence == "") {
                            keySequence = model.defaultShortcut
                        }
                        collectionModel.setShortcut(model.actionName, keySequence);
                    }
                }
            }
        }
    }
}
