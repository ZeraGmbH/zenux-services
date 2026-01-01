file(GLOB SOURCES
    *.cpp
    )

file(GLOB HEADER
    *.h
    )

add_executable(coredump-collector
    ${HEADER}
    ${SOURCES}
    )

target_link_libraries(coredump-collector
    PUBLIC
    ${QTX}::Core
    systemd
    coredump-collector-lib
    )

install(TARGETS coredump-collector
    EXPORT coredump-collectorExport
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
