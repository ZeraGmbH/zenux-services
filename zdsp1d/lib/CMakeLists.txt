set(SOURCES
    dsp-memory-management/dspvarclientperspective.cpp
    dsp-memory-management/dspvarparser.cpp
    dsp-memory-management/dspvarresolver.cpp
    cmdinterpret.cpp
    cmds.cpp
    dsp.cpp
    scpi-zdsp.cpp
    zdsp1d.cpp
    zdspclient.cpp
    zhserver.cpp
    )

set(HEADER
    dsp-memory-management/dspvarclientperspective.h
    dsp-memory-management/dspvardefinitions.h
    dsp-memory-management/dspvarparser.h
    dsp-memory-management/dspvarresolver.h
    cmdinterpret.h
    dsp.h
    dsp1scpi.h
    scpi-zdsp.h
    zdsp1d.h
    zdspglobal.h
    zeraglobal.h
    zdspclient.h
    zhserver.h
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
