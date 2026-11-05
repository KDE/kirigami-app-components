/*
 * SPDX-FileCopyrightText: 2011-2014 Sebastian Kügler <sebas@kde.org>
 * SPDX-FileCopyrightText: 2021 Devin Lin <devin@kde.org>
 * SPDX-License-Identifier: GPL-2.0-or-later
*/

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import org.kde.kirigamiaddons.private
import org.kde.kirigami as Kirigami
import org.kde.kirigami.settings

Kirigami.ScrollablePage {
    id: root

    required property string defaultModule
    required property list<ConfigurationModule> modules
    required property Kirigami.ApplicationWindow window

    // Do not use Map, it crashes very frequently
    property var pageCache: Object.create(null)

    ConfigurationSearch {
        id: configurationSearch
        modules: root.modules
        pageCache: root.pageCache
        pageParent: root
        currentPage: root.window.pageStack.layers.currentItem === root ? null : root.window.pageStack.layers.currentItem
        filterText: searchField.text
        onResultAccepted: module => root.openModule(module)
    }

    property bool initDone: false

    title: I18n.i18nc("@title", "Settings")

    // The layers are not busy when the push of this page is not animated.
    Component.onCompleted: Qt.callLater(root.openDefaultModule)

    Connections {
        target: root.window.pageStack.layers

        function onBusyChanged(): void {
            root.openDefaultModule();
        }
    }

    function openDefaultModule(): void {
        if (root.initDone || root.window.pageStack.layers.busy) {
            return;
        }
        root.initDone = true;
        const module = root.getModuleByName(root.defaultModule);
        const page = module ? root.pageForModule(module) : null;
        if (page) {
            root.window.pageStack.layers.push(page);
        }
    }

    ColumnLayout {
        spacing: 0

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

                    title: categoryDelegate.modelData.name === "_main_category" ? root.title : categoryDelegate.modelData.name

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
            text: configurationSearch.indexing
                ? I18n.i18nc("@info:placeholder", "Searching…")
                : I18n.i18nc("@info:placeholder", "No matching settings")
        }
    }

    // The static form action can trigger more than once in a row; avoid
    // pushing the same page twice.
    property Item _pendingPage: null

    function openModule(module: ConfigurationModule): void {
        const page = pageForModule(module);
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

    function pageForModule(module: ConfigurationModule, parent = root): var {
        if (pageCache[module.moduleId]) {
            return pageCache[module.moduleId];
        } else {
            const component = module.page();
            if (!component) {
                console.error("Unable to create component for moduleId", module.moduleId);
                return null;
            }
            if (component.status !== Component.Ready) {
                console.error(component.errorString());
                return null;
            }
            const page = component.createObject(parent, module.initialProperties());
            if (!page) {
                return null;
            }
            if (page.title.length === 0) {
                page.title = module.text;
            }
            pageCache[module.moduleId] = page;
            return page;
        }
    }

    function getModuleByName(moduleId: string): ConfigurationModule {
        return modules.find(module => module.moduleId == moduleId) ?? null;
    }
}
