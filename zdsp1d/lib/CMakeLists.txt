set(SOURCES
    dsp-memory-management/dspvarclientperspective.cpp
    dsp-memory-management/dspvarparser.cpp
    dsp-memory-management/dspvarresolver.cpp
    dsp.cpp
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
