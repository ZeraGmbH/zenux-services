add_executable(mt310s2d
    main.cpp
    )

target_link_libraries(mt310s2d
    PUBLIC
    mt310s2d-lib
    )

install(TARGETS mt310s2d
    EXPORT mt310s2dExport
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

add_subdirectory(configs)
add_subdirectory(lib)
add_subdirectory(systemd)
add_subdirectory(testlib)
add_subdirectory(tests)
