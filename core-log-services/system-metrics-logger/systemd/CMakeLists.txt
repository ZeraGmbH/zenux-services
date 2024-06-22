if(${useSystemd})
    set(SYSTEMD_UNITS
        system-metrics-logger.service
        )
    add_custom_target(system-metrics-logger-systemd-units ALL
        DEPENDS ${SYSTEMD_UNITS}
        SOURCES ${SYSTEMD_UNITS}
        COMMAND echo "Install systemd units..."
        )
    install(
        FILES ${SYSTEMD_UNITS}
        DESTINATION ${SYSTEMD_UNIT_INSTALL_PATH}
        )
endif()
