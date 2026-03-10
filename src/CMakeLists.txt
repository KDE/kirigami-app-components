
add_library(KirigamiActionCollection)
set_target_properties(KirigamiActionCollection PROPERTIES
    SOVERSION ${KF6KirigamiActionCollection_SOVERSION}
    VERSION ${KF6KirigamiActionCollection_VERSION}
)

target_compile_definitions(KirigamiActionCollection PRIVATE -DTRANSLATION_DOMAIN=\"kirigami-actioncollection\")

ecm_add_qml_module(KirigamiActionCollection
    URI "org.kde.kirigami.actioncollection"
    VERSION 1.0
    DEPENDENCIES QtQuick org.kde.kirigami
    GENERATE_PLUGIN_SOURCE
    INSTALLED_PLUGIN_TARGET KF6::KirigamiActionCollectionplugin
)

target_sources(KirigamiActionCollection PRIVATE
    actioncollection.cpp
    actiondata.cpp
    actionmodel.cpp
    keysequencehelper.cpp
    standardactioncollection.cpp
)

ecm_qt_declare_logging_category(KirigamiActionCollection
    HEADER loggingcategory.h
    IDENTIFIER KirigamiActionCollectionLog
    CATEGORY_NAME kf.kirigami.actioncollection
    DESCRIPTION "KirigamiActionCollection"
    DEFAULT_SEVERITY Warning
    EXPORT KIRIGAMI
)

ecm_target_qml_sources(KirigamiActionCollection SOURCES
    qml/ActionCollectionManager.qml
    qml/KQuickCommandBarPage.qml
    qml/KeySequenceItem.qml
    qml/ShortcutsEditor.qml
)

set_target_properties(KirigamiActionCollection PROPERTIES
    VERSION     ${PROJECT_VERSION}
    SOVERSION   6
    EXPORT_NAME "KirigamiActionCollection"
)

ecm_generate_export_header(KirigamiActionCollection
    BASE_NAME KirigamiActionCollection
    GROUP_BASE_NAME KF
    VERSION ${KF_VERSION}
    DEPRECATED_BASE_VERSION 0
    DEPRECATION_VERSIONS
    EXCLUDE_DEPRECATED_BEFORE_AND_AT ${EXCLUDE_DEPRECATED_BEFORE_AND_AT}
)

ecm_generate_qdoc(KirigamiActionCollection kirigamiactioncollection.qdocconf)

target_include_directories(KirigamiActionCollection PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)

target_link_libraries(KirigamiActionCollection PUBLIC
    Qt6::Core
    Qt6::Qml
    Qt6::Quick
PRIVATE
    Qt6::QuickControls2
    KF6::ConfigCore
    KF6::ConfigGui
    KF6::GuiAddons
    KF6::I18n
)

ecm_finalize_qml_module(KirigamiActionCollection DESTINATION ${KDE_INSTALL_QMLDIR} EXPORT KF6KirigamiActionCollectionTargets)

install(TARGETS KirigamiActionCollection EXPORT KF6KirigamiActionCollectionTargets ${KF_INSTALL_DEFAULT_ARGUMENTS})
