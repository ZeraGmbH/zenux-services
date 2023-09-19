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

option(useSystemd "Using libsystemd" ON)

find_package(Qt5 COMPONENTS Core Xml Network Test CONFIG REQUIRED)
find_package(SCPI REQUIRED)
find_package(ZenuxCore REQUIRED)
find_package(ZeraMicroController REQUIRED)
find_package(VeinFramework REQUIRED)

if(${useSystemd})
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(Systemd REQUIRED libsystemd)
    add_definitions(-DSYSTEMD_NOTIFICATION)
    # better suggestions?
    get_filename_component(SYSTEMD_UNIT_INSTALL_PATH "${CMAKE_INSTALL_PREFIX}/../lib/systemd/system" ABSOLUTE)
endif()

macro(SET_SERVICE_ENV SERVICE_NAME)
    set(SERVICE_CONFIG_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${SERVICE_NAME}/configs")

    # Pass source paths to configs for tests. This is module-global to allow e.g
    # scpimodule running tests with other modules
    string(TOUPPER ${SERVICE_NAME} SERVICE_NAME_UPPER)
    add_compile_definitions(CONFIG_SOURCES_${SERVICE_NAME_UPPER}="${SERVICE_CONFIG_PATH}")

    # xml-schema resources are required by modules and tests. If we put resource into
    # our library both would need some call of Q_INIT_RESOURCE. So publish resource
    # location and compile in where needed.

    # xsd are (not yet) inresource
    #set(XML_SCHEMA_RESOURCE_${SERVICE_NAME} ${SERVICE_CONFIG_PATH}/xml-schema.qrc)
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
            senserangetesttemplate
            zenuxservicecommon-testlib
            Zera::zera-timers-testlib
            Zera::zera-i2c-devices-testlib
            )
    endforeach()
endfunction()

add_subdirectory(com5003d)
add_subdirectory(mt310s2d)
add_subdirectory(scripts)
add_subdirectory(sec1000d)
add_subdirectory(zdsp1d)
add_subdirectory(zenux-service-common)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
