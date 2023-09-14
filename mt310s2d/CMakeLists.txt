set(SOURCES
    src/micro-controller-io/atmelsysctrl.cpp
    src/scpi-interfaces/clampinterface.cpp
    src/scpi-interfaces/samplinginterface.cpp
    src/scpi-interfaces/senseinterface.cpp
    src/scpi-interfaces/systeminterface.cpp
    src/scpi-interfaces/accumulatorinterface.cpp
    src/settings/ctrlsettings.cpp
    src/settings/accumulatorsettings.cpp
    src/senserange.cpp
    src/sensechannel.cpp
    src/adjustment.cpp
    src/mt310s2systeminfo.cpp
    src/clampjustdata.cpp
    src/adjflash.cpp
    src/clamp.cpp
    src/adjxml.cpp
    src/mt310s2d.cpp
    src/mt310s2justdata.cpp
    )

set(HEADER
    src/micro-controller-io/atmelsysctrl.h
    src/scpi-interfaces/clampinterface.h
    src/scpi-interfaces/samplinginterface.h
    src/scpi-interfaces/senseinterface.h
    src/scpi-interfaces/systeminterface.h
    src/scpi-interfaces/accumulatorinterface.h
    src/settings/ctrlsettings.h
    src/settings/accumulatorsettings.h
    src/senserange.h
    src/sensechannel.h
    src/adjflash.h
    src/adjxml.h
    src/adjustment.h
    src/mt310s2systeminfo.h
    src/clampjustdata.h
    src/clamp.h
    src/mt310s2d.h
    src/mt310s2dglobal.h
    src/mt310s2justdata.h
    )

add_library(mt310s2d-lib STATIC
    ${HEADER}
    ${SOURCES}
    )

target_include_directories(mt310s2d-lib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/micro-controller-io>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/scpi-interfaces>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/settings>
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
    Zera::zera-xmlconfig
    Zera::zera-i2c-devices
    Zera::zeraprotobuf
    zenuxservicecommon
    )

add_executable(mt310s2d
    src/main.cpp
    )

target_link_libraries(mt310s2d
    PUBLIC
    mt310s2d-lib
    )

install(TARGETS mt310s2d
    EXPORT mt310s2dExport
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

if(${useSystemd})
    set(SYSTEMD_UNITS
        systemd/mt310s2d.service
        systemd/mt310s2d.socket
        )
    add_custom_target(mt310s2d-systemd-units ALL
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
    add_subdirectory(testlib)
    add_subdirectory(tests)
endif()
