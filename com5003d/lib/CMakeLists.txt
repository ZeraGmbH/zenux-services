set(SOURCES
    scpi-interfaces/senseinterface.cpp
    scpi-interfaces/systeminterface.cpp
    com5003d.cpp
    com5003justdata.cpp
    senserange.cpp
    sensechannel.cpp
    adjustment.cpp
    )

set(HEADER
    scpi-interfaces/senseinterface.h
    scpi-interfaces/systeminterface.h
    com5003d.h
    com5003justdata.h
    com5003scpi.h
    com5003dglobal.h
    senserange.h
    sensechannel.h
    adjflash.h
    adjxml.h
    adjustment.h
    )

add_library(com5003d-lib STATIC
    ${HEADER}
    ${SOURCES}
    )

target_include_directories(com5003d-lib
    PUBLIC
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
    Zera::zera-xmlconfig
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