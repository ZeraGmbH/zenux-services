add_executable(sec1000d
    main.cpp
    )

target_link_libraries(sec1000d
    PUBLIC
    sec1000d-lib
    )

install(TARGETS sec1000d
    EXPORT sec1000dExport
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

add_subdirectory(configs)
add_subdirectory(lib)
add_subdirectory(systemd)
add_subdirectory(tests)
