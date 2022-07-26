file(GLOB SOURCES
    *.cpp 
    )


file(GLOB HEADER
    *.h  
    )

set(CONFIG_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/configs/zdsp1d.xml
)
set(CONFIG_VALIDATOR
    ${CMAKE_CURRENT_SOURCE_DIR}/configs/zdsp1d.xsd
)

# check for valid config files / display in qt-creator
add_custom_target(zdsp1d-configs ALL
    DEPENDS ${CONFIG_VALIDATOR} ${CONFIG_FILES}
    SOURCES ${CONFIG_VALIDATOR} ${CONFIG_FILES}
    COMMAND xmllint --noout --schema ${CONFIG_VALIDATOR} ${CONFIG_FILES}
    )


add_library(zdsp1d-lib STATIC
    ${HEADER}
    ${SOURCES}
    )

target_include_directories(zdsp1d-lib
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
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
    VeinMeta::xiqnet
    VeinMeta::zeraprotobuf
    ZeraClasses::zeraxmlconfig
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

install(
    FILES ${CONFIG_FILES} ${CONFIG_VALIDATOR}
    DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/zera/zdsp1d
    )

if(${useSystemd})
    set(SYSTEMD_UNITS
        systemd/zdsp1d.service
        systemd/zdsp1d.socket
        )
    add_custom_target(zdsp1d-systemd-units ALL
        DEPENDS ${SYSTEMD_UNITS}
        SOURCES ${SYSTEMD_UNITS}
        COMMAND echo "Install systemd units..."
        )
    install(
        FILES ${SYSTEMD_UNITS}
        DESTINATION ${SYSTEMD_UNIT_INSTALL_PATH}
        )
endif()
