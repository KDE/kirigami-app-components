# SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
# SPDX-FileCopyrightText: 2026 Marco Martin <mart@kde.org>
# SPDX-License-Identifier: BSD-2-Clause

set(_settings_private_deps org.kde.kirigami)

if (NOT ANDROID)
    list(APPEND _settings_private_deps org.kde.sonnet)
endif()

ecm_add_qml_module(settingsprivateplugin
    URI "org.kde.kirigami.settings.private"
    GENERATE_PLUGIN_SOURCE
    DEPENDENCIES ${_settings_private_deps}
)

ecm_target_qml_sources(settingsprivateplugin SOURCES
    ConfigWindow.qml
    ConfigMobilePage.qml
    ConfigurationSearch.qml
)

if (NOT ANDROID)
    ecm_target_qml_sources(settingsprivateplugin SOURCES
        SonnetConfigPage.qml
    )
endif()

target_link_libraries(settingsprivateplugin PRIVATE
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Qml
)

ecm_finalize_qml_module(settingsprivateplugin DESTINATION ${KDE_INSTALL_QMLDIR} EXPORT KirigamiAppComponentsTargets)
