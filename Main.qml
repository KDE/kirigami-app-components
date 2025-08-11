import QtQuick
import org.kde.kirigami as Kirigami
import actioncollection as AC

Kirigami.ApplicationWindow {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")

    pageStack {
        initialPage: testPage
    }

    Kirigami.Page {
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


    }
}
