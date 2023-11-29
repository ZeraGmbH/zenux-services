set(SOURCES
    cmdinterpret.cpp
    cmds.cpp
    dsp.cpp
    dspvarclientperspective.cpp
    dspvarparser.cpp
    dspvarresolver.cpp
    scpi-zdsp.cpp
    zdsp1d.cpp
    zdspclient.cpp
    zhserver.cpp
    )

set(HEADER
    cmdinterpret.h
    dsp.h
    dsp1scpi.h
    dspvarclientperspective.h
    dspvardefinitions.h
    dspvarparser.h
    dspvarresolver.h
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

add_executable(zdsp1d
    main.cpp
    )

target_link_libraries(zdsp1d
    PUBLIC
    zdsp1d-lib
    )

install(TARGETS zdsp1d
    EXPORT zdsp1dExport
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

add_subdirectory(configs)
add_subdirectory(systemd)
add_subdirectory(testlib)
add_subdirectory(tests)
