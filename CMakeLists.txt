cmake_minimum_required(VERSION 3.1)

project(zenux-services LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(PROJECT_VERSION_MAJOR "2")
set(PROJECT_VERSION_MINOR "0")
set(PROJECT_VERSION_PATCH "0")
set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

include(FeatureSummary)
include(GNUInstallDirs)
include(GenerateExportHeader)
include(CMakePackageConfigHelpers)
include(FindPkgConfig)

option(useSystemd "Using libsystemd" ON)
option(GUI_SIMULATION "Build QML gui code for simulation - NOT FOR TARGET" OFF)

find_package(Qt5 COMPONENTS Core Xml Network Test CONFIG REQUIRED)
find_package(SCPI REQUIRED)
find_package(ZenuxCore REQUIRED)
find_package(ZeraMicroController REQUIRED)
find_package(VeinFramework REQUIRED)
find_package(ZenuxLegacyComponents REQUIRED)
find_package(simplecmdiobase REQUIRED)

if(GUI_SIMULATION)
    message("Building with QML gui code for simulation")
    find_package(Qt5 COMPONENTS Qml Quick CONFIG REQUIRED)
    add_definitions(-DGUI_SIMULATION=TRUE)
else()
    message("Not building QML gui code for simulation")
endif()

if(${useSystemd})
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(Systemd REQUIRED libsystemd)
    add_definitions(-DSYSTEMD_NOTIFICATION)
    # better suggestions?
    get_filename_component(SYSTEMD_UNIT_INSTALL_PATH "${CMAKE_INSTALL_PREFIX}/../lib/systemd/system" ABSOLUTE)
endif()

macro(SET_SERVICE_ENV SERVICE_NAME)
    set(SERVICE_CONFIG_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${SERVICE_NAME}/configs")
    set(SERVICE_CONFIG_TARGET_DIR "/etc/zera/${SERVICE_NAME}/")

    # Pass source paths to configs for tests. This is module-global to allow e.g
    # scpimodule running tests with other modules
    string(TOUPPER ${SERVICE_NAME} SERVICE_NAME_UPPER)
    add_compile_definitions(CONFIG_SOURCES_${SERVICE_NAME_UPPER}="${SERVICE_CONFIG_PATH}")
    add_compile_definitions(CONFIG_TARGET_DIR_${SERVICE_NAME_UPPER}="${SERVICE_CONFIG_TARGET_DIR}")
endmacro()

SET_SERVICE_ENV(com5003d)
SET_SERVICE_ENV(mt310s2d)
SET_SERVICE_ENV(sec1000d)
SET_SERVICE_ENV(zdsp1d)

enable_testing()
# Inspired by
# https://github.com/KDAB/kdabtv/blob/master/Qt-Widgets-and-more/UnitTests/Tests/CMakeLists.txt
function(SETUP_TESTS)
    foreach(_testname ${ARGN})
        add_test(NAME ${_testname}
                 COMMAND ${_testname})
        add_executable(${_testname}
            ${_testname}.h
            ${_testname}.cpp
            )
        target_include_directories(${_testname}
            PRIVATE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
            ${PROJECT_SOURCE_DIR}/src
            )
        target_link_libraries(${_testname}
            PUBLIC
            Qt5::Test
            Zera::testlib-common
            Zera::zera-timers-testlib
            Zera::zera-i2c-devices-testlib
            Zera::resman-lib
            )
    endforeach()
endfunction()

add_subdirectory(com5003d)
add_subdirectory(core-log-services)
add_subdirectory(mt310s2d)
add_subdirectory(scripts)
add_subdirectory(sec1000d)
add_subdirectory(zdsp1d)
add_subdirectory(zenux-service-common)
add_subdirectory(zenux-service-interfaces)

write_basic_package_version_file(
    ZenuxServicesConfigVersion.cmake
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY SameMajorVersion
    )

configure_file(ZenuxServicesConfig.cmake.in ZenuxServicesConfig.cmake @ONLY)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/ZenuxServicesConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/ZenuxServicesConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ZenuxServices
    )

install(EXPORT ZenuxServicesExport
    FILE ZenuxServicesTargets.cmake
    NAMESPACE Zera::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ZenuxServices
    )

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
