set(SOURCES
    src/micro-controller-io/atmel.cpp
    src/micro-controller-io/atmelemobcontroller.cpp
    src/micro-controller-io/atmelsysctrl.cpp
    src/scpi-interfaces/clampinterface.cpp
    src/scpi-interfaces/emobinterface.cpp
    src/scpi-interfaces/frqinputinterface.cpp
    src/scpi-interfaces/hkeyinterface.cpp
    src/scpi-interfaces/samplinginterface.cpp
    src/scpi-interfaces/scheadinterface.cpp
    src/scpi-interfaces/senseinterface.cpp
    src/scpi-interfaces/sourceinterface.cpp
    src/scpi-interfaces/statusinterface.cpp
    src/scpi-interfaces/systeminterface.cpp
    src/settings/ctrlsettings.cpp
    src/settings/i2csettings.cpp
    src/settings/sensesettings.cpp
    src/settings/sourcesettings.cpp
    src/pcbserver.cpp
    src/samplerange.cpp
    src/senserange.cpp
    src/sensechannel.cpp
    src/fpzchannel.cpp
    src/adjustment.cpp
    src/systeminfo.cpp
    src/fpzinchannel.cpp
    src/scheadchannel.cpp
    src/clampjustdata.cpp
    src/adjflash.cpp
    src/clamp.cpp
    src/adjxml.cpp
    src/mt310s2d.cpp
    src/mt310s2justdata.cpp
    src/hkeychannel.cpp
    )

set(HEADER
    src/micro-controller-io/atmelbaseinterface.h
    src/micro-controller-io/atmelemobcontroller.h
    src/micro-controller-io/atmel.h
    src/micro-controller-io/atmelsysctrl.h
    src/scpi-interfaces/clampinterface.h
    src/scpi-interfaces/emobinterface.h
    src/scpi-interfaces/frqinputinterface.h
    src/scpi-interfaces/hkeyinterface.h
    src/scpi-interfaces/samplinginterface.h
    src/scpi-interfaces/scheadinterface.h
    src/scpi-interfaces/senseinterface.h
    src/scpi-interfaces/sourceinterface.h
    src/scpi-interfaces/statusinterface.h
    src/scpi-interfaces/systeminterface.h
    src/settings/ctrlsettings.h
    src/settings/i2csettings.h
    src/settings/sensesettings.h
    src/settings/sourcesettings.h
    src/pcbserver.h
    src/samplerange.h
    src/senserange.h
    src/sensechannel.h
    src/fpzchannel.h
    src/adjflash.h
    src/adjxml.h
    src/adjustment.h
    src/systeminfo.h
    src/notificationdata.h
    src/fpzinchannel.h
    src/scheadchannel.h
    src/clampjustdata.h
    src/clamp.h
    src/mt310s2d.h
    src/mt310s2dglobal.h
    src/mt310s2justdata.h
    src/hkeychannel.h
    )

set(CONFIG_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/configs/mt310s2d.xml
)
set(CONFIG_VALIDATOR
    ${CMAKE_CURRENT_SOURCE_DIR}/configs/mt310s2d.xsd
)

# check for valid config files / display in qt-creator
add_custom_target(mt310s2d-configs ALL
    DEPENDS ${CONFIG_VALIDATOR} ${CONFIG_FILES}
    SOURCES ${CONFIG_VALIDATOR} ${CONFIG_FILES}
    COMMAND xmllint --noout --schema ${CONFIG_VALIDATOR} ${CONFIG_FILES}
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
        PUBLIC
        systemd
    )
endif()

target_link_libraries(mt310s2d-lib
    PUBLIC
    Qt5::Core
    Qt5::Xml
    Qt5::Network
    Zera::zera-proxy
    Zera::zera-xmlconfig
    Zera::zera-i2c-devices
    VeinMeta::zeraprotobuf
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

install(
    FILES ${CONFIG_FILES} ${CONFIG_VALIDATOR}
    DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/zera/mt310s2d
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

add_subdirectory(tests)
