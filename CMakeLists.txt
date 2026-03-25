# SPDX-FileCopyrightText: 2026 Marco Martin <notmart@gmail.com>
# SPDX-License-Identifier: BSD-3-Clause
cmake_minimum_required(VERSION 3.16)

set(KF_VERSION "6.21.0") # handled by release scripts
set(KF_DEP_VERSION "6.20.0") # handled by release scripts

project(KirigamiActionCollection VERSION ${KF_VERSION})

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FeatureSummary)
find_package(ECM 6.20.0 NO_MODULE)
set_package_properties(ECM PROPERTIES TYPE REQUIRED DESCRIPTION "Extra CMake Modules." URL "https://commits.kde.org/extra-cmake-modules")
feature_summary(WHAT REQUIRED_PACKAGES_NOT_FOUND FATAL_ON_MISSING_REQUIRED_PACKAGES)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake )

include(KDEInstallDirs)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)
include(KDECMakeSettings)
include(KDEGitCommitHooks)

include(ECMGenerateExportHeader)
include(ECMGenerateHeaders)
include(ECMAddTests)
include(ECMSetupVersion)
include(ECMQtDeclareLoggingCategory)
include(ECMQmlModule)
include(ECMPoQmTools)
include(ECMDeprecationSettings)
include(ECMGenerateQDoc)

option(BUILD_EXAMPLES "Build example applications" OFF)

set(REQUIRED_QT_VERSION 6.8.0)
find_package(Qt6 6.5 REQUIRED COMPONENTS Quick Widgets QuickControls2)
find_package(KF6 ${KF_DEP_VERSION} REQUIRED COMPONENTS Config GuiAddons KirigamiPlatform I18n)

if (BUILD_TESTING)
    find_package(Qt6QuickTest ${REQUIRED_QT_VERSION} CONFIG QUIET)
endif()

set(EXCLUDE_DEPRECATED_BEFORE_AND_AT 0 CACHE STRING "Control the range of deprecated API excluded from the build [default=0].")

ecm_set_disabled_deprecation_versions(
    QT 6.10.0
)

add_subdirectory(src)

# TODO
# if(BUILD_EXAMPLES)
#     add_subdirectory(examples)
# endif()
#
# if(BUILD_TESTING)
#     add_subdirectory(autotests)
# endif()

if (BUILD_TESTING)
    add_subdirectory(autotests)
endif()

install(EXPORT KF6KirigamiActionCollectionTargets DESTINATION "${KDE_INSTALL_CMAKEPACKAGEDIR}/KF6KirigamiActionCollection"
        FILE KF6KirigamiActionCollectionTargets.cmake NAMESPACE KF6:: COMPONENT Devel)

include(ECMFeatureSummary)
ecm_feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)

