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
                AC.ActionCollection.collection: "pollo"
            },
            Kirigami.Action {
                text: "world"
            }
        ]
    }
}
