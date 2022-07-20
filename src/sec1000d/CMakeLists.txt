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
    resource.cpp
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
    resource.h
    systeminfo.h
    notificationdata.h
    ecalcsettings.h
    ecalcinterface.h
    ecalcchannel.h
    inputsettings.h 
    )

set(OTHER_FILES 
    sec1000d.xsd
    sec1000d.xml
)

add_executable(sec1000d
    ${SOURCES}
    ${HEADER}
    )

if(${Systemd_FOUND})
    target_link_libraries(sec1000d
        PRIVATE
        systemd
    )
endif()

target_link_libraries(sec1000d
    PRIVATE
    Qt5::Xml
    Qt5::Network
    Qt5::Gui
    Zera::SCPI
    VeinMeta::xiqnet
    Zera::scpi-tcp
    ZeraClasses::zeramisc 
    ZeraClasses::zeraxmlconfig 
    ZeraClasses::zeramath 
    VeinMeta::VfProtobuf
    VeinMeta::zeraprotobuf 
    zeraservercommon
    )

target_include_directories(sec1000d
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    )

install(TARGETS sec1000d
    EXPORT sec1000dExport
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

install(
    FILES ${OTHER_FILES}
    DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/zera/sec1000d
    )

