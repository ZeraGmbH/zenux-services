set(CONFIG_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/mt310s2d.xml
    ${CMAKE_CURRENT_SOURCE_DIR}/mt581s2d.xml
)
set(CONFIG_VALIDATOR
    ${CMAKE_CURRENT_SOURCE_DIR}/mtXXXs2d.xsd
)

# check for valid config files / display in qt-creator
add_custom_target(mt310s2d-configs ALL
    DEPENDS ${CONFIG_VALIDATOR} ${CONFIG_FILES}
    SOURCES ${CONFIG_VALIDATOR} ${CONFIG_FILES}
    COMMAND xmllint --noout --schema ${CONFIG_VALIDATOR} ${CONFIG_FILES}
    )

install(
    FILES ${CONFIG_FILES} ${CONFIG_VALIDATOR}
    DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/zera/mt310s2d
    )

