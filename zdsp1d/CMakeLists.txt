add_executable(zdsp1d
    main.cpp
    )

target_link_libraries(zdsp1d
    PUBLIC
    zdsp1d-lib
    )

install(TARGETS zdsp1d
    EXPORT zdsp1dExport
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

add_subdirectory(configs)
add_subdirectory(lib)
add_subdirectory(systemd)
add_subdirectory(testlib)
add_subdirectory(tests)
