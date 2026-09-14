add_executable(wait-for-relay-controller
    main.cpp
    )

target_link_libraries(wait-for-relay-controller
    PRIVATE
    zenuxservicecommon
    )

install(TARGETS wait-for-relay-controller
    EXPORT wait-for-relay-controllerExport
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )


set(SYSTEMD_UNITS
    wait-for-relay-controller.service
    )
add_custom_target(wait-for-relay-controller-systemd ALL
    DEPENDS ${SYSTEMD_UNITS}
    SOURCES ${SYSTEMD_UNITS}
    COMMAND echo "Install systemd units..."
    )
install(
    FILES ${SYSTEMD_UNITS}
    DESTINATION ${SYSTEMD_UNIT_INSTALL_PATH}
    )
