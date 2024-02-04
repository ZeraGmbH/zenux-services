file(GLOB SOURCES
    *.cpp
    device-nodes/*.cpp
    resources/*.cpp
    scpi-interfaces/*.cpp
    settings/*.cpp
    )

file(GLOB HEADER
    *.h
    device-nodes/*.h
    resources/*.h
    scpi-interfaces/*.h
    settings/*.h
    )

add_library(sec1000d-lib STATIC
    ${HEADER}
    ${SOURCES}
)

target_include_directories(sec1000d-lib
    PUBLIC
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/sec1000d-lib>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/device-nodes>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/resources>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/scpi-interfaces>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/settings>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

if(${Systemd_FOUND})
    target_link_libraries(sec1000d-lib
        PRIVATE
        systemd
    )
endif()

target_link_libraries(sec1000d-lib
    PUBLIC
    Qt5::Xml
    Qt5::Network
    Zera::zera-xmlconfig
    Zera::zeraprotobuf
    zenuxservicecommon
    )

set_target_properties(sec1000d-lib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(sec1000d-lib PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

export(TARGETS sec1000d-lib NAMESPACE Zera:: FILE ZenuxServicesTargets.cmake)
install(TARGETS sec1000d-lib
    EXPORT ZenuxServicesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/sec1000d-lib
    )

