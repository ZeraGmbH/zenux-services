add_executable(mt310s2-hotplug-device-detect
    main.cpp
    )

target_link_libraries(mt310s2-hotplug-device-detect
    PUBLIC
    mt310s2d-lib
    zenuxservicecommon-demolib
    )

install(TARGETS mt310s2-hotplug-device-detect
    EXPORT mt310s2dExport
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

add_definitions(-DINSTALL_PREFIX="${CMAKE_INSTALL_PREFIX}")
