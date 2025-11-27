cmake_minimum_required(VERSION 3.16)

project(actioncollection VERSION 0.1 LANGUAGES CXX)
set(KF_DEP_VERSION "6.17.0") # handled by release scripts

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(AUTOMOC_MOC_OPTIONS -Muri=org.kde.kirigami.actioncollection)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
find_package(ECM 6.17.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})

include(FeatureSummary)
include(KDEInstallDirs)
find_package(Qt6 6.5 REQUIRED COMPONENTS Quick Widgets QuickControls2)
find_package(KF6 ${KF_DEP_VERSION} REQUIRED COMPONENTS Config KirigamiPlatform I18n)

set_package_properties(ECM PROPERTIES TYPE REQUIRED DESCRIPTION "Extra CMake Modules." URL "https://commits.kde.org/extra-cmake-modules")

include(ECMGenerateExportHeader)
include(ECMSetupVersion)
include(ECMGenerateHeaders)
include(CMakePackageConfigHelpers)
include(ECMPoQmTools)
include(ECMFindQmlModule)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDEGitCommitHooks)
include(ECMQtDeclareLoggingCategory)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)
include(KDEPackageAppTemplates)
include(ECMQmlModule)
include(ECMDeprecationSettings)
include(ECMGenerateQDoc)


add_subdirectory(src)
