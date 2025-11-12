add_executable(sec1000d
    main.cpp
    )

target_link_libraries(sec1000d
    PRIVATE
    sec1000d-lib
    service-interfaces
    Zera::zenux-device-info
    )

install(TARGETS sec1000d
    EXPORT sec1000dExport
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

