file(GLOB SOURCES
    *.cpp
    )

file(GLOB HEADER
    *.h
    )

add_library(auto-journal-logger-lib STATIC
    ${HEADER}
    ${SOURCES}
)

target_include_directories(auto-journal-logger-lib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(auto-journal-logger-lib
    PUBLIC
    Qt5::Core
    Zera::zera-timers
    )

set_target_properties(auto-journal-logger-lib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(auto-journal-logger-lib PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})
