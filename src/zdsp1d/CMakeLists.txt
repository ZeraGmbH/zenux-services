file(GLOB SOURCES
    *.cpp 
    )


file(GLOB HEADER
    *.h  
    )

set(CONFIG_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/settings/zdsp1d.xml
)
set(CONFIG_VALIDATOR
    ${CMAKE_CURRENT_SOURCE_DIR}/settings/zdsp1d.xsd
)

# check for valid config files / display in qt-creator
add_custom_target(zdsp1d-configs ALL
    DEPENDS ${CONFIG_VALIDATOR} ${CONFIG_FILES}
    SOURCES ${CONFIG_VALIDATOR} ${CONFIG_FILES}
    COMMAND xmllint --noout --schema ${CONFIG_VALIDATOR} ${CONFIG_FILES}
    )

add_executable(zdsp1d
    ${SOURCES}
    ${HEADER}
    )

target_link_libraries(zdsp1d
    PUBLIC
    Qt5::Xml
    Qt5::Network
    Qt5::Core
    VeinMeta::xiqnet
    VeinMeta::zeraprotobuf
    ZeraClasses::zeraxmlconfig
    zeraservercommon
    )

if(${Systemd_FOUND})
    target_link_libraries(zdsp1d
        PRIVATE
        systemd
    )
endif()

target_include_directories(zdsp1d
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    )

install(TARGETS zdsp1d
    EXPORT zdsp1dExport
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

install(
    FILES ${CONFIG_FILES} ${CONFIG_VALIDATOR}
    DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/zera/zdsp1d
    )

