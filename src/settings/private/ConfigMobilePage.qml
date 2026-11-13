/*
 * SPDX-FileCopyrightText: 2011-2014 Sebastian Kügler <sebas@kde.org>
 * SPDX-FileCopyrightText: 2021 Devin Lin <devin@kde.org>
 * SPDX-FileCopyrightText: 2026 Marco Martin <mart@kde.org>
 * SPDX-License-Identifier: LGPL-2.0-or-later
*/

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import org.kde.ki18n
import org.kde.kirigami as Kirigami
import org.kde.kirigami.settings

Kirigami.ScrollablePage {
    id: root

    required property string defaultModule
    required property list<ConfigurationModule> modules
    required property Kirigami.ApplicationWindow window

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
        onResultAccepted: module => root.openModule(module)
    }

    // The layers are not busy when the push of this page is not animated.
    Component.onCompleted: Qt.callLater(root.openDefaultModule)

    Connections {
        target: root.window.pageStack.layers

        function onBusyChanged(): void {
            root.openDefaultModule();
        }
    }

    function openDefaultModule(): void {
        if (layout.initDone || root.window.pageStack.layers.busy) {
            return;
        }
        layout.initDone = true;
        const module = pageFactory.moduleById(root.modules, root.defaultModule);
        const page = module ? pageFactory.pageForModule(module, root) : null;
        if (page) {
            root.window.pageStack.layers.push(page);
        }
    }

    ColumnLayout {
        id: layout
        spacing: 0

        property bool initDone: false

        // search bar
        Kirigami.SearchField {
            id: searchField
            Layout.fillWidth: true
            Layout.margins: Kirigami.Units.largeSpacing
            autoAccept: false
            onAccepted: configurationSearch.acceptFirstResult()
            Keys.onDownPressed: nextItemInFocusChain(true)?.forceActiveFocus(Qt.TabFocusReason)
        }

        Kirigami.Form {
            Layout.fillWidth: true

            Repeater {
                model: {
                    let filteredCategories = new Array();

                    for (const module of configurationSearch.filteredModules) {
                        const category = filteredCategories.find((category) => category.name === module.category);
                        if (category) {
                            category.modules.push(module);
                        } else {
                            filteredCategories.push({
                                name: module.category,
                                modules: [module],
                            });
                        }
                    }
                    return filteredCategories;
                }

                delegate: Kirigami.FormGroup {
                    id: categoryDelegate

                    required property var modelData

                    title: categoryDelegate.modelData.name.length === 0 ? root.title : categoryDelegate.modelData.name

                    Repeater {
                        model: categoryDelegate.modelData.modules

                        delegate: Kirigami.FormAction {
                            id: moduleDelegate

                            required property ConfigurationModule modelData

                            action: Kirigami.Action {
                                text: moduleDelegate.modelData.text
                                icon.name: moduleDelegate.modelData.icon.name
                                onTriggered: root.openModule(moduleDelegate.modelData)
                            }
                        }
                    }
                }
            }
        }

        Kirigami.PlaceholderMessage {
            Layout.alignment: Qt.AlignHCenter
            Layout.margins: Kirigami.Units.gridUnit
            visible: configurationSearch.noResults
            text: _tr.i18nc("@info:placeholder", "No matching settings")
        }
    }

    // The static form action can trigger more than once in a row; avoid
    // pushing the same page twice.
    property Item _pendingPage: null

    function openModule(module: ConfigurationModule): void {
        const page = pageFactory.pageForModule(module, root);
        if (!page || root._pendingPage === page) {
            return;
        }
        root._pendingPage = page;
        root.window.pageStack.layers.push(page);
        Qt.callLater(() => {
            if (root._pendingPage === page) {
                root._pendingPage = null;
            }
        });
    }
}
