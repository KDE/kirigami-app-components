/*
 *  SPDX-FileCopyrightText: 2026 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQml
import QtTest
import org.kde.kirigami as Kirigami
import org.kde.kirigami.settings
import org.kde.kirigami.settings.private as Private

TestCase {
    id: testCase
    name: "Settings"

    width: 400
    height: 400
    visible: true

    Kirigami.ApplicationWindow {
        id: testWindow

        width: 800
        height: 600

        Component {
            id: generalPage

            Kirigami.Page {
                title: "General"
            }
        }

        SettingsViewController {
            id: settingsViewController
            window: testWindow
            modules: [
                SettingsModule {
                    moduleId: "general"
                    name: "General"
                    page: generalPage
                }
            ]
        }
    }

    SettingsModule {
        id: appearanceModule
        moduleId: "appearance"
        name: "Appearance"
        page: null
        keywords: ["theme"]
    }

    SettingsModule {
        id: aboutModule
        moduleId: "about"
        name: "About"
        page: null
        keywords: ["credits", "dark"]
    }

    SettingsModule {
        id: hiddenModule
        moduleId: "hidden"
        name: "Hidden"
        page: null
        visible: false
    }

    Private.ConfigurationSearch {
        id: search
        modules: [appearanceModule, aboutModule, hiddenModule]
    }

    Component {
        id: dummyPageComponent

        QtObject {
            property string title: ""
        }
    }

    SettingsModule {
        id: pageModule
        moduleId: "paged"
        name: "Paged"
        page: dummyPageComponent
    }

    Private.PageFactory {
        id: pageFactory
    }

    // An unknown defaultModule falls back to the first module on desktop
    Component {
        id: pagedPageComponent

        Kirigami.Page {
            title: ""
        }
    }

    SettingsModule {
        id: pagedPageModule
        moduleId: "paged-page"
        name: "Paged"
        page: pagedPageComponent
    }

    SignalSpy {
        id: acceptSpy
        target: search
        signalName: "resultAccepted"
    }

    function init() {
        // Emptying the filter synchronously resets the search
        search.filterText = "";
        compare(search.searchText, "");
        acceptSpy.clear();
    }

    function test_allVisibleModulesListed() {
        compare(search.filteredModules.length, 2);
        compare(search.filteredModules[0], appearanceModule);
        compare(search.filteredModules[1], aboutModule);
        verify(!search.noResults);
    }

    function test_searchIsDebounced() {
        search.filterText = "appearance";

        // The search only runs after the debounce timer fired
        compare(search.searchText, "");
        tryVerify(() => search.searchText === "appearance");

        compare(search.filteredModules.length, 1);
        compare(search.filteredModules[0], appearanceModule);
    }

    function test_searchIsCaseInsensitive() {
        // searchText keeps the raw filter; lowercasing happens while matching
        search.filterText = "APPEARANCE";
        tryVerify(() => search.searchText === "APPEARANCE");

        compare(search.filteredModules.length, 1);
        compare(search.filteredModules[0], appearanceModule);
    }

    function test_allTermsMustMatch() {
        // A term matches the module text or any of its keywords
        search.filterText = "dark";
        tryVerify(() => search.searchText === "dark");
        compare(search.filteredModules.length, 1);
        compare(search.filteredModules[0], aboutModule);

        // All terms must match somewhere in the same module
        search.filterText = "appearance theme";
        tryVerify(() => search.searchText === "appearance theme");
        compare(search.filteredModules.length, 1);
        compare(search.filteredModules[0], appearanceModule);

        // No single module matches both terms
        search.filterText = "dark theme";
        tryVerify(() => search.searchText === "dark theme");
        compare(search.filteredModules.length, 0);
        verify(search.noResults);
    }

    function test_acceptFirstResult() {
        search.filterText = "appearance";

        // acceptFirstResult bypasses the debounce timer
        search.acceptFirstResult();
        compare(search.searchText, "appearance");

        compare(acceptSpy.count, 1);
        compare(acceptSpy.signalArguments[0][0], appearanceModule);

        // A query without matches does not emit the signal
        search.filterText = "nonexistent";
        search.acceptFirstResult();
        compare(acceptSpy.count, 1);
    }

    function test_pageFactory() {
        const page = pageFactory.pageForModule(pageModule, null);
        verify(page !== null);

        // The module text is used as fallback title
        compare(page.title, "Paged");

        // The page is cached: a second call returns the same instance
        compare(pageFactory.pageForModule(pageModule, null), page);

        compare(pageFactory.moduleById([pageModule], "paged"), pageModule);
        compare(pageFactory.moduleById([pageModule], "nonexistent"), null);

        page.destroy();
    }

    function test_openDesktop() {
        testWindow.show();
        settingsViewController.open();

        // configViewItem is no longer public; the test (which already imports
        // the private module) can still reach it through the private __d.
        const view = settingsViewController.__d.configViewItem;
        verify(view !== null);

        // On desktop the settings open as a dialog layer: the dialog window's
        // page stack holds the sidebar (titled "Settings") and, once the
        // default module is pushed, its page on top.
        compare(view.pageStack.get(0).title, "Settings");
        tryVerify(() => view.pageStack.depth === 2);
        tryVerify(() => {
            const page = view.pageStack.currentItem as Kirigami.Page;
            return page !== null && page.title === "General";
        });

        // Opening again reuses the existing view
        settingsViewController.open();
        compare(settingsViewController.__d.configViewItem, view);

        // Closing destroys the view and clears the private configViewItem
        view.close();
        tryVerify(() => settingsViewController.__d.configViewItem === null);
    }
}
