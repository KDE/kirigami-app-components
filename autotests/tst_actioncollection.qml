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
                    toolTip: "This is a tooltip"
                    icon.name: "document-send"
                    defaultShortcut: "Ctrl+H"
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
        target: helloAction
        signalName: "triggered"
    }
    SignalSpy {
        id: spyHelloKirigamiAction
        target: helloKirigamiAction
        signalName: "triggered"
    }

    function initTestCase() {
        mainWindow.show();
    }

    function cleanupTestCase() {
        mainWindow.close();
    }

    function test_1same_properties() {
        compare(helloAction.text, helloKirigamiAction.text);
        compare(helloAction.icon.name, helloKirigamiAction.icon.name);
        compare(helloAction.toolTip, helloKirigamiAction.tooltip);
    }

    function test_2shortcut_trigger() {
        keySequence("Ctrl+H");
        spyHelloKirigamiAction.wait();
        spyHelloAction.wait();
    }

    function test_3reassign_properties() {
        helloAction.text = "New Text";
        helloAction.icon.name = "go-back";
        helloAction.toolTip = "New Tooltip";
        helloAction.defaultShortcut = "Ctrl+J";

        compare(helloAction.text, helloKirigamiAction.text);
        compare(helloAction.icon.name, helloKirigamiAction.icon.name);
        compare(helloAction.toolTip, helloKirigamiAction.tooltip);
        // We don't have the shortcut overridden, so the action shortcut will change as well
        compare(helloAction.defaultShortcut, helloKirigamiAction.shortcut);

        // New shortcut will work
        keySequence("Ctrl+J");
        spyHelloKirigamiAction.wait();
        spyHelloAction.wait();
    }

    function test_4manual_trigger_propagation() {
        helloKirigamiAction.trigger();
        spyHelloAction.wait();

        //FIXME: for now trigger only propagates from the kirigami action to the qaction and not the other way around or it could do a signal loop
        //helloAction.trigger();
        //spyHelloKirigamiAction.wait();
    }
    function findKeySequenceItems(item, out, visited) {
        if (item === null || item === undefined) {
            return;
        }
        for (var i = 0; i < visited.length; ++i) {
            if (visited[i] === item) {
                return;
            }
        }
        visited.push(item);
        // Composite type names look like "KeySequenceItem_QML_94(...)"
        if (item.toString().indexOf("KeySequenceItem") === 0) {
            out.push(item);
        }
        // NB: plain C++ QObjects have neither data nor children;
        // Dialog content additionally lives under contentItem/contentData
        var kids = [];
        function pushAll(list) {
            if (list === undefined || list === null) {
                return;
            }
            for (var j = 0; j < list.length; ++j) {
                kids.push(list[j]);
            }
        }
        if (item.data !== undefined) {
            pushAll(item.data);
        } else if (item.children !== undefined) {
            pushAll(item.children);
        }
        if (item.contentItem !== undefined && item.contentItem !== null) {
            kids.push(item.contentItem);
        }
        if (item.contentData !== undefined) {
            pushAll(item.contentData);
        }
        for (var k = 0; k < kids.length; ++k) {
            findKeySequenceItems(kids[k], out, visited);
        }
    }

    function test_5shortcutsEditor_modifierless() {
        var component = Qt.createComponent("org.kde.kirigami.actioncollection", "ShortcutsEditor");
        compare(component.status, Component.Ready, component.errorString());
        // Parent to the window so the dialog's Overlay.overlay parent exists
        // and the dialog lands in the window tree
        var editor = component.createObject(mainWindow);
        verify(editor !== null);
        try {
            // Default off: recording behavior is unchanged unless opted in
            compare(editor.allowModifierlessShortcuts, false);
            var items = [];
            findKeySequenceItems(mainWindow, items, []);
            // One item for "Shortcut:", one for "Alternative:"
            compare(items.length, 2);
            verify(!items[0].modifierlessAllowed);
            verify(!items[1].modifierlessAllowed);
            // Opt-in propagates to both fields
            editor.allowModifierlessShortcuts = true;
            verify(items[0].modifierlessAllowed);
            verify(items[1].modifierlessAllowed);
        } finally {
            editor.destroy();
        }
    }
}
