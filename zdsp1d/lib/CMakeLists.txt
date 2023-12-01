set(SOURCES
    dsp-memory-management/dspvarclientperspective.cpp
    dsp-memory-management/dspvarparser.cpp
    dsp-memory-management/dspvarresolver.cpp
    dsp.cpp
    dspsettings.cpp
    scpicmdinterpreter.cpp
    scpicmds.cpp
    scpi-zdsp.cpp
    zdspserver.cpp
    zdspclient.cpp
    )

set(HEADER
    dsp-memory-management/dspvarclientperspective.h
    dsp-memory-management/dspvardefinitions.h
    dsp-memory-management/dspvarparser.h
    dsp-memory-management/dspvarresolver.h
    dsp.h
    dspsettings.h
    scpicmdinterpreter.h
    scpicmds.h
    scpi-zdsp.h
    zdspserver.h
    zeraglobal.h
    zdspclient.h
    )

add_library(zdsp1d-lib STATIC
    ${HEADER}
    ${SOURCES}
    )

target_include_directories(zdsp1d-lib
    PUBLIC
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

generate_export_header(zdsp1d-lib)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/zdsp1d-lib_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zdsp1d-lib
    )

export(TARGETS zdsp1d-lib NAMESPACE Zera:: FILE ZenuxServicesTargets.cmake)
install(TARGETS zdsp1d-lib
    EXPORT ZenuxServicesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zdsp1d-lib
    )
