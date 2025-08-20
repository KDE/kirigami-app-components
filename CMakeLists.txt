cmake_minimum_required(VERSION 3.16)

project(actioncollection VERSION 0.1 LANGUAGES CXX)
set(KF_DEP_VERSION "6.17.0") # handled by release scripts

set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(ECM 6.17.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})

find_package(Qt6 6.5 REQUIRED COMPONENTS Quick Widgets)
find_package(KF6 ${KF_DEP_VERSION} REQUIRED COMPONENTS Config KirigamiPlatform I18n)

qt_standard_project_setup(REQUIRES 6.5)

qt_add_executable(appactioncollection
    actioncollection.cpp
    actiondata.cpp
    #shortcutsmodel.cpp
    main.cpp
)

qt_add_resources(appactioncollection "resources"
    PREFIX "/"
    FILES
        actionsrc
)

qt_add_qml_module(appactioncollection
    URI actioncollection
    VERSION 1.0
    QML_FILES
        Main.qml
)



target_link_libraries(appactioncollection PUBLIC
        Qt6::Widgets
        KF6::ConfigCore
        KF6::ConfigGui
        KF6::I18n
)

target_link_libraries(appactioncollection
    PRIVATE Qt6::Quick
)

include(GNUInstallDirs)
install(TARGETS appactioncollection
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
