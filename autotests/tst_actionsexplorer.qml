// SPDX-FileCopyrightText: 2026 Eike Hein <hein@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later

import QtQuick
import QtTest
import org.kde.kirigami as Kirigami
import org.kde.kirigami.actioncollection as AC

TestCase {
    id: testCase
    name: "ActionsExplorer"
    when: windowShown

    AC.ActionCollection {
        name: "explorer-test"
        AC.ActionData { name: "invoke"; text: "Invoke" }
    }

    Component {
        id: windowComponent
        Kirigami.ApplicationWindow {
            id: window
            width: 600
            height: 400
            visible: true
            property int invocations: 0
            property alias actionEnabled: action.enabled
            property alias actionVisible: action.visible

            Kirigami.Action {
                AC.ActionCollection.collection: "explorer-test"
                AC.ActionCollection.action: "invoke"
                enabled: false
                onTriggered: testCase.fail("A disabled instance was triggered")
            }
            Kirigami.Action {
                id: action
                AC.ActionCollection.collection: "explorer-test"
                AC.ActionCollection.action: "invoke"
                onTriggered: ++window.invocations
            }
        }
    }

    Component {
        id: explorerComponent
        AC.ActionsExplorer {}
    }

    Component {
        id: modelComponent
        AC.ActionModel {}
    }

    // Filtering can follow a different window and releases a destroyed window.
    function test_windowLifetime() {
        const first = createTemporaryObject(windowComponent, testCase);
        const second = createTemporaryObject(windowComponent, testCase);
        const model = createTemporaryObject(modelComponent, testCase);
        const index = model.index(0, 0);
        compare(model.data(index, AC.ActionModel.ActionInstancesRole).length, 4);
        model.window = first;
        compare(model.data(model.index(0, 0), AC.ActionModel.ActionInstancesRole).length, 2);
        model.window = second;
        compare(model.data(model.index(0, 0), AC.ActionModel.ActionInstancesRole).length, 2);
        second.destroy();
        tryCompare(model, "window", null);
        compare(model.data(model.index(0, 0), AC.ActionModel.ActionInstancesRole).length, 2);
    }

    function test_dispatch_data() {
        return [
            {tag: "click", keyboard: false, otherWindow: false},
            {tag: "keyboard", keyboard: true, otherWindow: false},
            {tag: "other-window-click", keyboard: false, otherWindow: true},
            {tag: "other-window-keyboard", keyboard: true, otherWindow: true}
        ];
    }

    // Search results invoke an enabled instance in the selected window.
    function test_dispatch(data) {
        const first = createTemporaryObject(windowComponent, testCase);
        const second = createTemporaryObject(windowComponent, testCase);
        const host = data.otherWindow ? first : second;
        const explorer = createTemporaryObject(explorerComponent, host.contentItem);
        if (data.otherWindow) {
            explorer.actionWindow = second;
        }
        compare(explorer.actionWindow, second);
        host.requestActivate();
        tryCompare(host, "active", true);
        explorer.open();
        tryCompare(explorer, "opened", true);
        explorer.text = "Invoke";
        tryCompare(explorer, "count", 1);
        tryVerify(() => explorer.currentItem !== null);
        if (data.keyboard) {
            keyClick(Qt.Key_Return);
        } else {
            mouseClick(explorer.currentItem);
        }
        tryCompare(second, "invocations", 1);
        compare(first.invocations, 0);
        tryCompare(explorer, "visible", false);
    }

    // An instance in another window cannot make an unavailable local action appear.
    function test_unavailable_data() {
        return [{tag: "disabled", enabled: false, visible: true}, {tag: "hidden", enabled: true, visible: false}];
    }

    function test_unavailable(data) {
        const first = createTemporaryObject(windowComponent, testCase);
        const second = createTemporaryObject(windowComponent, testCase, {actionEnabled: data.enabled, actionVisible: data.visible});
        const explorer = createTemporaryObject(explorerComponent, second.contentItem);
        explorer.open();
        tryCompare(explorer, "opened", true);
        explorer.text = "Invoke";
        tryCompare(explorer, "count", 0);
        explorer.close();
        compare(first.invocations, 0);
        compare(second.invocations, 0);
    }
}
