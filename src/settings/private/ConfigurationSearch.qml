// SPDX-FileCopyrightText: 2026 Carl Schwan <carl@carlschwan.eu>
// SPDX-FileCopyrightText: 2026 Marco Martin <mart@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

pragma ComponentBehavior: Bound

import QtQuick
import org.kde.kirigami.settings

QtObject {
    id: root

    required property list<ConfigurationModule> modules
    property string filterText
    property string searchText

    signal resultAccepted(module: ConfigurationModule)

    onFilterTextChanged: {
        if (filterText.trim().length === 0) {
            queryTimer.stop();
            searchText = "";
        } else {
            queryTimer.restart();
        }
    }

    readonly property Timer queryTimer: Timer {
        interval: 150
        onTriggered: root.searchText = root.filterText.trim()
    }

    readonly property bool noResults: root.searchText.length > 0 && root.filteredModules.length === 0

    // Views recreate all their delegates on model changes, so only update when needed.
    property list<ConfigurationModule> filteredModules: []

    onModulesChanged: __updateSearch()
    onSearchTextChanged: __updateSearch()

    Component.onCompleted: __updateSearch()

    function __updateSearch(): void {
        const matching = root.modules.filter(module => root.matchesModule(module));
        if (!root.sameObjects(root.filteredModules, matching)) {
            root.filteredModules = matching;
        }
    }

    function matchesModule(module: ConfigurationModule): bool {
        if (!module.visible) {
            return false;
        }

        const pattern = root.searchText.trim();
        if (pattern.length === 0) {
            return true;
        }
        if (module.searchable && root.matches(pattern, module.text)) {
            return true;
        }
        return module.keywords.some(keyword => root.matches(pattern, keyword));
    }

    function acceptFirstResult(): void {
        queryTimer.stop();
        root.searchText = root.filterText.trim();
        const firstModule = root.modules.find(module => root.matchesModule(module));
        if (firstModule) {
            root.resultAccepted(firstModule);
        }
    }

    function matches(pattern: string, text: string): bool {
        return (text ?? "").toLowerCase().includes(pattern.toLowerCase());
    }

    function sameObjects(a: var, b: var): bool {
        return a.length === b.length && a.every((object, i) => object === b[i]);
    }
}
