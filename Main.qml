// SPDX-License-Identifier: LGPL-2.0-or-later
// SPDX-FileCopyrightText: 2025 Marco Martin <notmart@gmail.com>

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC
import org.kde.kirigami as Kirigami
import actioncollection as AC
import org.kde.kquickcontrols as KQ

Kirigami.ApplicationWindow {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")

    pageStack {
        initialPage: testPage
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
            }
        ]

        ListView {
            model: AC.ActionCollectionModel {
                id: collectionModel
                name: "org.kde.collection"
            }
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
