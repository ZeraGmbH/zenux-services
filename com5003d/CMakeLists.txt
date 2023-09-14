set(SOURCES
    scpi-interfaces/samplinginterface.cpp
    scpi-interfaces/senseinterface.cpp
    scpi-interfaces/systeminterface.cpp
    main.cpp
    com5003d.cpp
    com5003justdata.cpp
    senserange.cpp
    sensechannel.cpp
    adjustment.cpp
    )

set(HEADER
    scpi-interfaces/samplinginterface.h
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
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/micro-controller-io>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/scpi-interfaces>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/settings>
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

add_executable(com5003d
    main.cpp
    )

target_link_libraries(com5003d
    PUBLIC
    com5003d-lib
    )

install(TARGETS com5003d
    EXPORT com5003dExport
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

if(${useSystemd})
    set(SYSTEMD_UNITS
        systemd/com5003d.service
        systemd/com5003d.socket
        )
    add_custom_target(com5003d-systemd-units ALL
        DEPENDS ${SYSTEMD_UNITS}
        SOURCES ${SYSTEMD_UNITS}
        COMMAND echo "Install systemd units..."
        )
    install(
        FILES ${SYSTEMD_UNITS}
        DESTINATION ${SYSTEMD_UNIT_INSTALL_PATH}
        )
endif()

add_subdirectory(configs)

if(NOT SKIP_UNITTESTS)
    add_subdirectory(tests)
endif()
