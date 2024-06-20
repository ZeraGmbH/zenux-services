file(GLOB SOURCES
    *.cpp
    )

file(GLOB HEADER
    *.h
    )

add_library(coredump-collector-lib STATIC
    ${HEADER}
    ${SOURCES}
)

target_include_directories(coredump-collector-lib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(coredump-collector-lib
    PUBLIC
    Qt5::Core
    Zera::zera-timers
    )

set_target_properties(coredump-collector-lib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(coredump-collector-lib PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})
