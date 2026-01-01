file(GLOB SOURCES
    *.cpp
    )

file(GLOB HEADER
    *.h
    )

add_executable(system-metrics-logger
    ${HEADER}
    ${SOURCES}
    )

target_link_libraries(system-metrics-logger
    PUBLIC
    ${QTX}::Core
    systemd
    system-metrics
    )

install(TARGETS system-metrics-logger
    EXPORT system-metrics-loggerExport
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
