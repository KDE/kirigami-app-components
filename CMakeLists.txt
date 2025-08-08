cmake_minimum_required(VERSION 3.16)

project(actioncollection VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Quick)

qt_standard_project_setup(REQUIRES 6.5)



qt_add_qml_module(appactioncollection_static
    STATIC
    URI actioncollection
    VERSION 1.0
    QML_FILES
        Main.qml
)

target_sources(appactioncollection_static PUBLIC
    actioncollection.cpp
)

qt_add_executable(appactioncollection
    main.cpp
)

target_link_libraries(appactioncollection PUBLIC appactioncollection_static appactioncollection_staticplugin)

target_link_libraries(appactioncollection
    PRIVATE Qt6::Quick
)

include(GNUInstallDirs)
install(TARGETS appactioncollection
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
