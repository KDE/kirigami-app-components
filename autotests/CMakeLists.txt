if(NOT TARGET Qt6::QuickTest)
    message(STATUS "Qt6QuickTest not found, autotests will not be built.")
    return()
endif()

add_executable(qmltest qmltest.cpp)
qt_add_qml_module(qmltest URI ActionCollectionTestUtils)
target_link_libraries(qmltest PRIVATE Qt6::Qml Qt6::QuickTest  KirigamiActionCollection)
if (NOT QT6_IS_SHARED_LIBS_BUILD OR NOT BUILD_SHARED_LIBS)
    target_link_libraries(qmltest PRIVATE KirigamiPolyfillplugin)
    qt6_import_qml_plugins(qmltest)
endif()

macro(kirigami_add_tests)
    if (WIN32)
        set(_extra_args -platform offscreen)
    endif()

    if (BUILD_SHARED_LIBS)
        set(_extra_args ${_extra_args} -import ${CMAKE_BINARY_DIR}/bin)
    endif()

    foreach(test ${ARGV})
        add_test(NAME ${test}
                 COMMAND qmltest
                        ${_extra_args}
                        -input ${test}
                 WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )
    endforeach()
endmacro()

kirigami_add_tests(
    tst_actioncollection.qml
)

set_tests_properties(
    tst_actioncollection.qml

    PROPERTIES
        RUN_SERIAL ON
)
