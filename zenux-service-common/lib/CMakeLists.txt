file(GLOB HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/adjustment/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/adjustment/*/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/async-notifications/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/channel-range-factories/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/device-nodes/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/meas-channels/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/micro-controller-io/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/resources/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/scpi-interfaces/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/scpi-protobuf/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/settings/*.h
    )

file(GLOB SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/adjustment/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/adjustment/*/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/async-notifications/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/channel-range-factories/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/device-nodes/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/meas-channels/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/micro-controller-io/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/resources/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/scpi-interfaces/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/scpi-protobuf/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/settings/*.cpp
    )

file(GLOB RESOURCES
)

add_library(zenuxservicecommon SHARED
    ${HEADERS}
    ${SOURCES}
    scpi-interfaces/resources/scpi_interfaces.qrc
)

target_link_libraries(zenuxservicecommon
    PUBLIC
    Qt5::Core
    Zera::xiqnet
    Zera::SCPI
    Zera::zera-timers
    Zera::zera-xmlconfig
    Zera::zera-math
    Zera::zera-i2c-devices
    source-json-io-lib
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
)

# announce headers - target perspective
target_include_directories(zenuxservicecommon
    PUBLIC
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/zenuxservicecommon>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/adjustment>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/adjustment/base>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/adjustment/range>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/adjustment/rangeitem>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/async-notifications>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/channel-range-factories>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/device-nodes>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/meas-channels>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/micro-controller-io>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/resources>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/scpi-interfaces>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/scpi-protobuf>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/settings>
    )

set_target_properties(zenuxservicecommon PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(zenuxservicecommon PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

install(TARGETS zenuxservicecommon
    EXPORT ZenuxServicesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

generate_export_header(zenuxservicecommon)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/zenuxservicecommon_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zenuxservicecommon
    )

export(TARGETS zenuxservicecommon FILE ZenuxServicesTargets.cmake)
install(
    FILES ${HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zenuxservicecommon
    )

