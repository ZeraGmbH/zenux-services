file(GLOB SOURCES
    *.cpp
    micro-controller-io/*.cpp
    scpi-interfaces/*.cpp
    )

file(GLOB HEADER
    *.h
    micro-controller-io/*.h
    scpi-interfaces/*.h
    )

add_library(mt310s2d-lib STATIC
    ${HEADER}
    ${SOURCES}
)

target_include_directories(mt310s2d-lib
    PUBLIC
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/mt310s2d-lib>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/micro-controller-io>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/scpi-interfaces>
    )

if(${Systemd_FOUND})
    target_link_libraries(mt310s2d-lib
        PRIVATE
        systemd
    )
endif()

target_link_libraries(mt310s2d-lib
    PUBLIC
    Qt5::Core
    Qt5::Xml
    Qt5::Network
    Zera::zera-i2c-devices
    Zera::zeraprotobuf
    system-metrics
    zenuxservicecommon
    )

set_target_properties(mt310s2d-lib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(mt310s2d-lib PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

export(TARGETS mt310s2d-lib NAMESPACE Zera:: FILE ZenuxServicesTargets.cmake)
install(TARGETS mt310s2d-lib
    EXPORT ZenuxServicesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mt310s2d-lib
    )
