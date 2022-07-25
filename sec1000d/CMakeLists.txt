set(SOURCES
    main.cpp
    sec1000d.cpp
    pcbserver.cpp
    ethsettings.cpp
    fpgasettings.cpp
    statusinterface.cpp
    systeminterface.cpp
    debugsettings.cpp
    systeminfo.cpp
    ecalcsettings.cpp
    ecalcinterface.cpp
    ecalcchannel.cpp
    inputsettings.cpp
    )

set(HEADER
    zeraglobal.h
    sec1000d.h
    sec1000dscpi.h
    pcbserver.h
    sec1000dglobal.h
    ethsettings.h
    fpgasettings.h
    statusinterface.h
    systeminterface.h
    debugsettings.h
    systeminfo.h
    notificationdata.h
    ecalcsettings.h
    ecalcinterface.h
    ecalcchannel.h
    inputsettings.h 
    )

set(CONFIG_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/configs/sec1000d.xml
)
set(CONFIG_VALIDATOR
    ${CMAKE_CURRENT_SOURCE_DIR}/configs/sec1000d.xsd
)

# check for valid config files / display in qt-creator
add_custom_target(sec1000d-configs ALL
    DEPENDS ${CONFIG_VALIDATOR} ${CONFIG_FILES}
    SOURCES ${CONFIG_VALIDATOR} ${CONFIG_FILES}
    COMMAND xmllint --noout --schema ${CONFIG_VALIDATOR} ${CONFIG_FILES}
    )


add_library(sec1000d-lib STATIC
    ${HEADER}
    ${SOURCES}
    )

target_include_directories(sec1000d-lib
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    )

if(${Systemd_FOUND})
    target_link_libraries(sec1000d-lib
        PRIVATE
        systemd
    )
endif()

target_link_libraries(sec1000d-lib
    PUBLIC
    Qt5::Xml
    Qt5::Network
    Qt5::Gui
    ZeraClasses::zeramisc 
    ZeraClasses::zeraxmlconfig 
    ZeraClasses::zeramath 
    VeinMeta::VfProtobuf
    VeinMeta::zeraprotobuf 
    zenuxservicecommon
    )

add_executable(sec1000d
    main.cpp
    )

target_link_libraries(sec1000d
    PUBLIC
    sec1000d-lib
    )

install(TARGETS sec1000d
    EXPORT sec1000dExport
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

install(
    FILES ${CONFIG_FILES} ${CONFIG_VALIDATOR}
    DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/zera/sec1000d
    )

if(${useSystemd})
    set(SYSTEMD_UNITS
        systemd/sec1000d.service
        systemd/sec1000d.socket
        )
    add_custom_target(sec1000d-systemd-units ALL
        DEPENDS ${SYSTEMD_UNITS}
        SOURCES ${SYSTEMD_UNITS}
        COMMAND echo "Install systemd units..."
        )
    install(
        FILES ${SYSTEMD_UNITS}
        DESTINATION ${SYSTEMD_UNIT_INSTALL_PATH}
        )
endif()

