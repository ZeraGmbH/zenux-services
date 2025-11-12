add_executable(com5003d
    main.cpp
    )

target_link_libraries(com5003d
    PUBLIC
    com5003d-lib
    )

install(TARGETS com5003d
    EXPORT com5003dExport
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

