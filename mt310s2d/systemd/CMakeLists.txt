if(${useSystemd})
    set(SYSTEMD_UNITS
        mt310s2d.service
        mt310s2d.socket
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
