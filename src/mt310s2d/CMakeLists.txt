set(SOURCES
    micro-controller-io/atmel.cpp
    micro-controller-io/atmelemobcontroller.cpp
    micro-controller-io/atmelsysctrl.cpp
    micro-controller-io/atmelwatcher.cpp
    scpi-interfaces/clampinterface.cpp
    scpi-interfaces/emobinterface.cpp
    scpi-interfaces/frqinputinterface.cpp
    scpi-interfaces/hkeyinterface.cpp
    scpi-interfaces/samplinginterface.cpp
    scpi-interfaces/scheadinterface.cpp
    scpi-interfaces/senseinterface.cpp
    scpi-interfaces/sourceinterface.cpp
    scpi-interfaces/statusinterface.cpp
    scpi-interfaces/systeminterface.cpp
    settings/ctrlsettings.cpp
    settings/debugsettings.cpp
    settings/ethsettings.cpp
    settings/fpgasettings.cpp
    settings/frqinputsettings.cpp
    settings/i2csettings.cpp
    settings/samplingsettings.cpp
    settings/scheadsettings.cpp
    settings/sensesettings.cpp
    settings/sourcesettings.cpp
    settings/hkeysettings.cpp
    justdata.cpp
    pcbserver.cpp
    samplerange.cpp
    senserange.cpp
    sensechannel.cpp
    fpzchannel.cpp
    adjustment.cpp
    systeminfo.cpp
    resource.cpp
    fpzinchannel.cpp
    scheadchannel.cpp
    clampjustdata.cpp
    adjflash.cpp
    clamp.cpp
    adjxml.cpp
    mt310s2d.cpp
    mt310s2justdata.cpp
    hkeychannel.cpp
    )

set(HEADER
    micro-controller-io/atmelbaseinterface.h
    micro-controller-io/atmelemobcontroller.h
    micro-controller-io/atmel.h
    micro-controller-io/atmelsysctrl.h
    micro-controller-io/atmelwatcher.h
    scpi-interfaces/clampinterface.h
    scpi-interfaces/emobinterface.h
    scpi-interfaces/frqinputinterface.h
    scpi-interfaces/hkeyinterface.h
    scpi-interfaces/samplinginterface.h
    scpi-interfaces/scheadinterface.h
    scpi-interfaces/senseinterface.h
    scpi-interfaces/sourceinterface.h
    scpi-interfaces/statusinterface.h
    scpi-interfaces/systeminterface.h
    settings/ctrlsettings.h
    settings/debugsettings.h
    settings/ethsettings.h
    settings/fpgasettings.h
    settings/frqinputsettings.h
    settings/hkeysettings.h
    settings/i2csettings.h
    settings/samplingsettings.h
    settings/scheadsettings.h
    settings/sensesettings.h
    settings/sourcesettings.h
    zeraglobal.h
    justdata.h
    mt310s2dscpi.h
    pcbserver.h
    resource.h
    samplerange.h
    senserange.h
    sensechannel.h
    fpzchannel.h
    adjflash.h
    adjxml.h
    adjustment.h
    systeminfo.h
    notificationdata.h
    fpzinchannel.h
    scheadchannel.h
    clampjustdata.h
    clamp.h
    mt310s2d.h
    mt310s2dglobal.h
    mt310s2justdata.h
    hkeychannel.h
    )

set(CONFIG_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/mt310s2d.xml
)
set(CONFIG_VALIDATOR
    ${CMAKE_CURRENT_SOURCE_DIR}/mt310s2d.xsd
)

# check for valid config files / display in qt-creator
add_custom_target(mt310s2-configs ALL
    DEPENDS ${CONFIG_VALIDATOR} ${CONFIG_FILES}
    SOURCES ${CONFIG_VALIDATOR} ${CONFIG_FILES}
    COMMAND xmllint --noout --schema ${CONFIG_VALIDATOR} ${CONFIG_FILES}
    )


add_library(mt310s2d_lib STATIC
    ${HEADER}
    ${SOURCES}
    )

target_include_directories(mt310s2d_lib
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    )

if(${Systemd_FOUND})
    target_link_libraries(mt310s2d_lib
        PUBLIC
        systemd
    )
endif()

target_link_libraries(mt310s2d_lib
    PUBLIC
    Qt5::Core
    Qt5::Gui
    Qt5::Xml
    Qt5::Network
    Zera::SCPI
    Zera::scpi-tcp
    VeinMeta::xiqnet
    ZeraClasses::zerai2c
    ZeraClasses::zeraproxy
    ZeraClasses::zeramisc
    ZeraClasses::zeraxmlconfig
    ZeraClasses::zeramath
    ZeraClasses::zeradev
    VeinMeta::VfProtobuf
    VeinMeta::zeraprotobuf
    zeraservercommon
    )

add_executable(mt310s2d
    main.cpp
    )

target_link_libraries(mt310s2d
    PUBLIC
    mt310s2d_lib
    )

install(TARGETS mt310s2d
    EXPORT mt310s2dExport
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

install(
    FILES ${CONFIG_FILES} ${CONFIG_VALIDATOR}
    DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/zera/mt310s2d
    )

