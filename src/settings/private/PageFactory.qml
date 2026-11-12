// SPDX-FileCopyrightText: 2026 Marco Martin <mart@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

pragma ComponentBehavior: Bound

import QtQml
import org.kde.kirigami.settings

QtObject {
    id: root

    // Do not use Map, it crashes very frequently
    property var pageCache: Object.create(null)

    function pageForModule(module: ConfigurationModule, parent: var): var {
        if (root.pageCache[module.moduleId]) {
            return root.pageCache[module.moduleId];
        }

        const component = typeof module.page === "function" ? module.page() : module.page;
        if (!component) {
            console.error("Unable to create component for moduleId", module.moduleId);
            return null;
        }

        if (component.status !== Component.Ready) {
            console.error(`Unable to create component for moduleId: "${module.moduleId}". Error: ${component.errorString()}`);
            return null;
        }

        const page = component.createObject(parent, module.initialProperties());
        if (!page) {
            return null;
        }

        if (page.title !== undefined && page.title.length === 0) {
            page.title = module.text;
        }
        root.pageCache[module.moduleId] = page;
        return page;
    }

    function moduleById(modules: list<ConfigurationModule>, moduleId: string): ConfigurationModule {
        return modules.find(module => module.moduleId === moduleId) ?? null;
    }
}
