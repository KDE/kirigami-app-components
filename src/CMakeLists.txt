
add_library(KirigamiActionCollection)
ecm_add_qml_module(KirigamiActionCollection URI "org.kde.kirigami.actioncollection"
    VERSION 2.0
    GENERATE_PLUGIN_SOURCE
    INSTALLED_PLUGIN_TARGET KF6KirigamiActionCollectionplugin
    DEPENDENCIES QtQuick org.kde.kirigami
)

ecm_qt_declare_logging_category(KirigamiActionCollection
    HEADER loggingcategory.h
    IDENTIFIER KirigamiActionCollectionLog
    CATEGORY_NAME kf.kirigami.actioncollection
    DESCRIPTION "KirigamiActionCollection"
    DEFAULT_SEVERITY Warning
    EXPORT KIRIGAMI
)

target_sources(KirigamiActionCollection PRIVATE
    actioncollection.cpp
    actiondata.cpp
    actionmodel.cpp
)

ecm_target_qml_sources(KirigamiActionCollection SOURCES
    qml/KQuickCommandBarPage.qml
    qml/KeySequenceItem.qml
    qml/ShortcutsEditor.qml
)

set_target_properties(KirigamiActionCollection PROPERTIES
    VERSION     ${PROJECT_VERSION}
    SOVERSION   6
    EXPORT_NAME "KirigamiActionCollection"
)

target_include_directories(KirigamiActionCollection PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)

target_link_libraries(KirigamiActionCollection PRIVATE Qt6::Quick Qt6::QuickControls2 KF6::ConfigCore KF6::ConfigGui KF6::I18n)

ecm_finalize_qml_module(KirigamiActionCollection EXPORT KirigamiTargets)

install(TARGETS KirigamiActionCollection EXPORT KirigamiTargets ${KF_INSTALL_DEFAULT_ARGUMENTS})
