if(${useSystemd})
    set(SYSTEMD_UNITS
        auto-journal-logger.service
        auto-journal-logger.socket
        )
    add_custom_target(auto-journal-logger-systemd-units ALL
        DEPENDS ${SYSTEMD_UNITS}
        SOURCES ${SYSTEMD_UNITS}
        COMMAND echo "Install systemd units..."
        )
    install(
        FILES ${SYSTEMD_UNITS}
        DESTINATION ${SYSTEMD_UNIT_INSTALL_PATH}
        )
endif()
