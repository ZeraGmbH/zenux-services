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

