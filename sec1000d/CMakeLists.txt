set(SOURCES
    main.cpp
    sec1000d.cpp
    statusinterface.cpp
    systeminterface.cpp
    systeminfo.cpp
    ecalcsettings.cpp
    ecalcinterface.cpp
    ecalcchannel.cpp
    inputsettings.cpp
    )

set(HEADER
    sec1000d.h
    sec1000dglobal.h
    statusinterface.h
    systeminterface.h
    systeminfo.h
    ecalcsettings.h
    ecalcinterface.h
    ecalcchannel.h
    inputsettings.h 
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
    Zera::zera-xmlconfig
    Zera::zeraprotobuf
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

add_subdirectory(configs)
add_subdirectory(tests)
