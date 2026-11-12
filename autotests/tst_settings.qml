/*
 *  SPDX-FileCopyrightText: 2026 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtTest
import org.kde.kirigami.settings
import org.kde.kirigami.settings.private as Private

TestCase {
    id: testCase
    name: "Settings"

    width: 400
    height: 400
    visible: true

    ConfigurationModule {
        id: appearanceModule
        moduleId: "appearance"
        text: "Appearance"
        page: () => null
        keywords: ["theme"]
    }

    ConfigurationModule {
        id: aboutModule
        moduleId: "about"
        text: "About"
        page: () => null
        searchable: false
        keywords: ["credits", "dark"]
    }

    ConfigurationModule {
        id: hiddenModule
        moduleId: "hidden"
        text: "Hidden"
        page: () => null
        visible: false
    }

    Private.ConfigurationSearch {
        id: search
        modules: [appearanceModule, aboutModule, hiddenModule]
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

    function test_unsearchableModuleOnlyMatchedByKeywords() {
        // The about module is not searchable: its text must not match...
        search.filterText = "about";
        tryVerify(() => search.searchText === "about");
        compare(search.filteredModules.length, 0);
        verify(search.noResults);

        // ...but its keywords are still searched
        search.filterText = "credits";
        tryVerify(() => search.searchText === "credits");
        compare(search.filteredModules.length, 1);
        compare(search.filteredModules[0], aboutModule);
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
}
