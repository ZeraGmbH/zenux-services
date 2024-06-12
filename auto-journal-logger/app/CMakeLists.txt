file(GLOB SOURCES
    *.cpp
    )

file(GLOB HEADER
    *.h
    )

add_executable(auto-journal-logger
    ${HEADER}
    ${SOURCES}
    )

target_link_libraries(auto-journal-logger
    PUBLIC
    Qt5::Core
    auto-journal-logger-lib
    )

install(TARGETS auto-journal-logger
    EXPORT auto-journal-loggerExport
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
