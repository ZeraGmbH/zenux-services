if(${useSystemd})
    set(SYSTEMD_UNITS
        zdsp1d.service
        zdsp1d.socket
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
