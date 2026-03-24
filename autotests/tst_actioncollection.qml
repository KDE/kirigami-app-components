/*
 *  SPDX-FileCopyrightText: 2026 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick
import QtTest
import org.kde.kirigami as Kirigami
import org.kde.kirigami.actioncollection as AC

TestCase {
    id: testCase
    name: "ActionCollection"

    width: 400
    height: 400
    visible: true

    function applicationWindow() { return mainWindow; }

    Kirigami.ApplicationWindow {
        id: mainWindow
        pageStack.initialPage: [mainPage]

        AC.ActionCollectionManager {
            id: manager
            pageRow: pageStack
            AC.ActionCollection {
                name: "TestCollection1"
                AC.ActionData {
                    id: helloAction
                    name: "hello"
                    text: "Hello"
                    icon.name: "document-send"
                    defaultShortcut: "Ctrl+H"
                        Component.onCompleted: print("BAH")
                }
            }
        }

        Kirigami.Page {
            id: mainPage
            actions: [
                Kirigami.Action {
                    id: helloKirigamiAction
                    AC.ActionCollection.collection: "TestCollection1"
                    AC.ActionCollection.action: "hello"
                    onTriggered: {
                        print("Hello action triggered")
                    }
                }
            ]
        }
    }

    SignalSpy {
        id: spyHelloAction
        target: helloKirigamiAction
        signalName: "triggered"
    }

    function initTestCase() {
        mainWindow.show();
    }

    function cleanupTestCase() {
        mainWindow.close();
    }

    function test_action_trigger() {
       // compare(helloAction.text, helloKirigamiAction.text);

       // keySequence("Ctrl+H");
        //spyHelloAction.wait();
    }
}
