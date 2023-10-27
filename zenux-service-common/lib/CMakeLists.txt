file(GLOB HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/meas-channels/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/micro-controller-io/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/resources/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/scpi-interfaces/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/scpi-protobuf/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/settings/*.h
    )

file(GLOB SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/meas-channels/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/micro-controller-io/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/resources/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/scpi-interfaces/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/scpi-protobuf/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/settings/*.cpp
    )

add_library(zenuxservicecommon SHARED
    ${HEADERS}
    ${SOURCES}
    )

target_link_libraries(zenuxservicecommon
    PUBLIC
    Qt5::Core
    Zera::xiqnet
    Zera::SCPI
    Zera::zera-xmlconfig
    Zera::zera-math
    Zera::zera-i2c-devices
    Zera::zera-timers
    Zera::service-common-code
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

# announce headers - target perspective
target_include_directories(zenuxservicecommon
    PUBLIC
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/zenuxservicecommon>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/meas-channels>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/micro-controller-io>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/resources>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/scpi-interfaces>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/scpi-protobuf>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/settings>
    )

set_target_properties(zenuxservicecommon PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(zenuxservicecommon PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

generate_export_header(zenuxservicecommon)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/zenuxservicecommon_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zenuxservicecommon
    )

install(TARGETS zenuxservicecommon
    EXPORT zenuxservicecommonExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

