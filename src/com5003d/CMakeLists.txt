set(SOURCES
    micro-controller-io/atmel.cpp
    micro-controller-io/atmelwatcher.cpp
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
    settings/hkeysettings.cpp
    settings/i2csettings.cpp
    settings/samplingsettings.cpp
    settings/scheadsettings.cpp
    settings/sensesettings.cpp
    settings/sourcesettings.cpp
    main.cpp
    com5003d.cpp
    justdata.cpp
    com5003justdata.cpp
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
    hkeychannel.cpp
    )

set(HEADER
    micro-controller-io/atmel.h
    micro-controller-io/atmelwatcher.h
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
    com5003d.h
    justdata.h
    com5003justdata.h
    hkeychannel.h
    com5003scpi.h
    pcbserver.h
    com5003dglobal.h
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
    )

set(CONFIG_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/com5003d.xml
    )
set(CONFIG_VALIDATOR
    ${CMAKE_CURRENT_SOURCE_DIR}/com5003d.xsd
    )

# check for valid config files / display in qt-creator
add_custom_target(com5003d-configs ALL
    DEPENDS ${CONFIG_VALIDATOR} ${CONFIG_FILES}
    SOURCES ${CONFIG_VALIDATOR} ${CONFIG_FILES}
    COMMAND xmllint --noout --schema ${CONFIG_VALIDATOR} ${CONFIG_FILES}
    )

add_executable(com5003d
    ${SOURCES}
    ${HEADER}
    )

if(${Systemd_FOUND})
    target_link_libraries(com5003d
        PRIVATE
        systemd
    )
endif()

target_link_libraries(com5003d
    PRIVATE
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

target_include_directories(com5003d
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    )

install(TARGETS com5003d
    EXPORT com5003dExport
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

install(
    FILES ${CONFIG_FILES} ${CONFIG_VALIDATOR}
    DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/zera/com5003d
    )

