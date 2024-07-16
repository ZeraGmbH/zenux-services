file(GLOB SOURCES
    *.cpp
    scpi-interfaces/*.cpp
    settings/*.cpp
    )

file(GLOB HEADER
    *.h
    scpi-interfaces/*.h
    settings/*.h
    )

add_library(com5003d-lib STATIC
    ${HEADER}
    ${SOURCES}
    )

target_include_directories(com5003d-lib
    PUBLIC
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/com5003d-lib>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/scpi-interfaces>
    )

if(${Systemd_FOUND})
    target_link_libraries(com5003d-lib
        PRIVATE
        systemd
    )
endif()

target_link_libraries(com5003d-lib
    PUBLIC
    Qt5::Core
    Qt5::Xml
    Qt5::Network
    Zera::zera-i2c-devices
    Zera::zeraprotobuf
    zenuxservicecommon
    )

set_target_properties(com5003d-lib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(com5003d-lib PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

export(TARGETS com5003d-lib NAMESPACE Zera:: FILE ZenuxServicesTargets.cmake)
install(TARGETS com5003d-lib
    EXPORT ZenuxServicesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/com5003d-lib
    )
