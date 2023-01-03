set(SOURCES
    micro-controller-io/atmel.cpp
    scpi-interfaces/frqinputinterface.cpp
    scpi-interfaces/hkeyinterface.cpp
    scpi-interfaces/samplinginterface.cpp
    scpi-interfaces/scheadinterface.cpp
    scpi-interfaces/senseinterface.cpp
    scpi-interfaces/sourceinterface.cpp
    scpi-interfaces/statusinterface.cpp
    scpi-interfaces/systeminterface.cpp
    settings/ethsettings.cpp
    settings/frqinputsettings.cpp
    settings/hkeysettings.cpp
    settings/i2csettings.cpp
    settings/samplingsettings.cpp
    settings/scheadsettings.cpp
    settings/sensesettings.cpp
    settings/sourcesettings.cpp
    main.cpp
    com5003d.cpp
    com5003justdata.cpp
    pcbserver.cpp
    samplerange.cpp
    senserange.cpp
    sensechannel.cpp
    fpzchannel.cpp
    adjustment.cpp
    systeminfo.cpp
    fpzinchannel.cpp
    scheadchannel.cpp
    hkeychannel.cpp
    )

set(HEADER
    micro-controller-io/atmel.h
    scpi-interfaces/frqinputinterface.h
    scpi-interfaces/hkeyinterface.h
    scpi-interfaces/samplinginterface.h
    scpi-interfaces/scheadinterface.h
    scpi-interfaces/senseinterface.h
    scpi-interfaces/sourceinterface.h
    scpi-interfaces/statusinterface.h
    scpi-interfaces/systeminterface.h
    settings/ethsettings.h
    settings/frqinputsettings.h
    settings/hkeysettings.h
    settings/i2csettings.h
    settings/samplingsettings.h
    settings/scheadsettings.h
    settings/sensesettings.h
    settings/sourcesettings.h
    zeraglobal.h
    com5003d.h
    com5003justdata.h
    hkeychannel.h
    com5003scpi.h
    pcbserver.h
    com5003dglobal.h
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
    ${CMAKE_CURRENT_SOURCE_DIR}/configs/com5003d.xml
    )
set(CONFIG_VALIDATOR
    ${CMAKE_CURRENT_SOURCE_DIR}/configs/com5003d.xsd
    )

# check for valid config files / display in qt-creator
add_custom_target(com5003d-configs ALL
    DEPENDS ${CONFIG_VALIDATOR} ${CONFIG_FILES}
    SOURCES ${CONFIG_VALIDATOR} ${CONFIG_FILES}
    COMMAND xmllint --noout --schema ${CONFIG_VALIDATOR} ${CONFIG_FILES}
    )


add_library(com5003d-lib STATIC
    ${HEADER}
    ${SOURCES}
    )

target_include_directories(com5003d-lib
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
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
    Zera::zera-proxy
    Zera::zera-xmlconfig
    Zera::zera-i2c-devices
    VeinMeta::VfProtobuf
    VeinMeta::zeraprotobuf
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

install(
    FILES ${CONFIG_FILES} ${CONFIG_VALIDATOR}
    DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/zera/com5003d
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
