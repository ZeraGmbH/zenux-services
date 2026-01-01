file(GLOB SOURCES
    *.cpp
    )

file(GLOB HEADER
    *.h
    )

add_library(auto-journal-logger-testlib STATIC
    ${HEADER}
    ${SOURCES}
    )

target_include_directories(auto-journal-logger-testlib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(auto-journal-logger-testlib
    PUBLIC
    ${QTX}::Core
    auto-journal-logger-lib
    )

set_target_properties(auto-journal-logger-testlib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(auto-journal-logger-testlib PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})
