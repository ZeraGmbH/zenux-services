set(SOURCES
    dsp-memory-management/dspvarclientperspective.cpp
    dsp-memory-management/dspvarresolver.cpp
    dsp.cpp
    dspsettings.cpp
    scpicmdinterpreter.cpp
    scpicmds.cpp
    scpiparserzdsp1d.cpp
    scpi-zdsp.cpp
    zdspserver.cpp
    zdspclient.cpp
    )

set(HEADER
    dsp-memory-management/dspvarclientperspective.h
    dsp-memory-management/dspvardefinitions.h
    dsp-memory-management/dspvarresolver.h
    dsp.h
    dspsettings.h
    scpicmdinterpreter.h
    scpicmds.h
    scpiparserzdsp1d.h
    scpi-zdsp.h
    zdspserver.h
    zdspclient.h
    )

add_library(zdsp1d-lib STATIC
    ${HEADER}
    ${SOURCES}
    )

target_include_directories(zdsp1d-lib
    PUBLIC
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/zdsp1d-lib>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/dsp-memory-management>
    )

if(${Systemd_FOUND})
    target_link_libraries(zdsp1d-lib
        PRIVATE
        systemd
    )
endif()

target_link_libraries(zdsp1d-lib
    PUBLIC
    Qt5::Xml
    Qt5::Network
    Qt5::Core
    Zera::xiqnet
    Zera::zeraprotobuf
    Zera::zera-xmlconfig
    zenuxservicecommon
    )

set_target_properties(zdsp1d-lib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(zdsp1d-lib PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

export(TARGETS zdsp1d-lib NAMESPACE Zera:: FILE ZenuxServicesTargets.cmake)
install(TARGETS zdsp1d-lib
    EXPORT ZenuxServicesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zdsp1d-lib
    )
