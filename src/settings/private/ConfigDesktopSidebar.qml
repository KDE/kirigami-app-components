// SPDX-FileCopyrightText: 2020 Tobias Fella <fella@posteo.de>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-FileCopyrightText: 2021 Felipe Kinoshita <kinofhek@gmail.com>
// SPDX-FileCopyrightText: 2021 Marco Martin <mart@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as QQC
import QtQuick.Layouts
import QtQuick.Window
import org.kde.ki18n
import org.kde.kirigami as Kirigami
import org.kde.kirigami.settings

Kirigami.ScrollablePage {
    id: root

    implicitWidth: Kirigami.Units.gridUnit * 13
    Kirigami.Theme.colorSet: Kirigami.Theme.View
    Kirigami.Theme.inherit: false

    Kirigami.ColumnView.interactiveResizeEnabled: true
    Kirigami.ColumnView.pinned: true
    Kirigami.ColumnView.minimumWidth: Kirigami.Units.gridUnit * 5
    Kirigami.ColumnView.maximumWidth: Kirigami.Units.gridUnit * 15
    //Kirigami.ColumnView.view.columnResizeMode: Kirigami.ColumnView.DynamicColumns

    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0

    property Kirigami.SearchField searchField
    titleDelegate: Kirigami.SearchField {
        id: searchField
        Layout.fillWidth: true
        autoAccept: false
        onAccepted: configurationSearch.acceptFirstResult()
        Keys.onDownPressed: listview.focusResult(0)
        anchors {
            verticalCenter: parent.verticalCenter
            left: parent.left
            right: parent.right
        }
        Binding {
            root.searchField: searchField
        }
    }

    required property string defaultModule
    required property list<SettingsModule> modules
    readonly property Kirigami.PageRow pageStack: root.Window.window?.pageStack

    property SettingsModule currentModule: null

    property bool initDone: false

    // Pushing the default page directly in Component.onCompleted would be too
    // early; Qt.callLater defers it to after the initial binding evaluation.
    Component.onCompleted: Qt.callLater(root.openDefaultModule)

    function openDefaultModule(): void {
        if (root.initDone) {
            return;
        }
        Kirigami.ColumnView.view.columnResizeMode = Kirigami.ColumnView.DynamicColumns
        root.initDone = true;
        // On desktop a missing or unknown defaultModule falls back to the
        // first module; on mobile the module list is simply shown instead.
        const module = pageFactory.moduleById(root.modules, root.defaultModule) ?? root.modules[0] ?? null;
        const page = module ? pageFactory.pageForModule(module, root.contentItem) : null;
        if (page) {
            print(root.Window.window)
            root.pageStack.push(page);
            root.currentModule = module;
        }
    }

    function openModule(module: SettingsModule): void {
        if (root.currentModule === module) {
            return;
        }
        const page = pageFactory.pageForModule(module, root.contentItem);
        if (!page) {
            return;
        }

        root.pageStack.pop(root);
        root.pageStack.push(page);

        root.currentModule = module;
    }

    KI18nContext {
        id: _tr
        translationDomain: "kirigami-app-components"
    }

    PageFactory {
        id: pageFactory
    }

    ConfigurationSearch {
        id: configurationSearch
        modules: root.modules
        filterText: searchField.text
        onResultAccepted: module => {
            const hadFocus = searchField.activeFocus;
            root.openModule(module);
            // Newly created pages take the focus.
            if (hadFocus) {
                searchField.forceActiveFocus();
            }
        }
    }

    ListView {
        id: listview

        function focusResult(index: int): void {
            if (index < 0) {
                searchField.forceActiveFocus(Qt.BacktabFocusReason);
            } else if (index < count) {
                positionViewAtIndex(index, ListView.Contain);
                itemAtIndex(index)?.forceActiveFocus(Qt.TabFocusReason);
            }
        }

        currentIndex: configurationSearch.filteredModules.indexOf(root.currentModule)

        model: configurationSearch.filteredModules
        delegate: QQC.ItemDelegate {
            id: settingDelegate
            width: listview.width

            required property int index
            required property SettingsModule modelData

            text: modelData?.name
            icon.name: modelData?.icon.name
            icon.source: modelData?.icon.source
            checked: root.currentModule === settingDelegate.modelData
            highlighted: checked

            Keys.onUpPressed: listview.focusResult(index - 1)
            Keys.onDownPressed: listview.focusResult(index + 1)
            Keys.onReturnPressed: clicked()
            Keys.onEnterPressed: clicked()

            onClicked: root.openModule(modelData)
        }

        Kirigami.PlaceholderMessage {
            parent: listview
            anchors.centerIn: parent
            width: parent.width - Kirigami.Units.gridUnit * 2
            visible: configurationSearch.noResults
            text: _tr.i18nc("@info:placeholder", "No matching settings")
        }
    }
}
