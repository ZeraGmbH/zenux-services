cmake_minimum_required(VERSION 3.1)

project(zenux-services LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PROJECT_VERSION_MAJOR "2")
set(PROJECT_VERSION_MINOR "0")
set(PROJECT_VERSION_PATCH "0")

set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

include(FeatureSummary)
include(GNUInstallDirs)
include(GenerateExportHeader)

option(useSystemd "Using libsystemd" ON)

find_package(Qt5 COMPONENTS Core Xml Network Test CONFIG REQUIRED)
find_package(SCPI REQUIRED)
find_package(xiqnet REQUIRED)
find_package(ZeraClasses REQUIRED)
find_package(zeraprotobuf REQUIRED)

enable_testing()

if(${useSystemd})
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(Systemd REQUIRED libsystemd)
    add_definitions(-DSYSTEMD_NOTIFICATION)
    # better suggestions?
    get_filename_component(SYSTEMD_UNIT_INSTALL_PATH "${CMAKE_INSTALL_PREFIX}/../lib/systemd/system" ABSOLUTE)
endif()

add_subdirectory(com5003d)
add_subdirectory(mt310s2d)
add_subdirectory(scripts)
add_subdirectory(sec1000d)
add_subdirectory(zdsp1d)
add_subdirectory(zenux-service-common)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
