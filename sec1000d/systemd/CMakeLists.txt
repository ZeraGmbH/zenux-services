if(${useSystemd})
    set(SYSTEMD_UNITS
        sec1000d.service
        sec1000d.socket
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
