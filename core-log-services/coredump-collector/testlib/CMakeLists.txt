file(GLOB SOURCES
    *.cpp
    )

file(GLOB HEADER
    *.h
    )

add_library(coredump-collector-testlib STATIC
    ${HEADER}
    ${SOURCES}
    )

target_include_directories(coredump-collector-testlib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(coredump-collector-testlib
    PUBLIC
    Qt5::Core
    coredump-collector-lib
    )

set_target_properties(coredump-collector-testlib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(coredump-collector-testlib PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})
