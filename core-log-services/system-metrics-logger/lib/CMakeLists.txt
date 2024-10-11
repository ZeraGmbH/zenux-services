file(GLOB SOURCES
    *.cpp
    log-components/*.cpp
    decoder/*.cpp
    )

file(GLOB HEADER
    *.h
    datatypes/*.h
    decoder/*.h
    log-components/*.h
    )

add_library(system-metrics STATIC
    ${SOURCES}
    ${HEADER}
)

target_link_libraries(system-metrics
    PUBLIC
    Qt5::Core
    Zera::zera-timers
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

target_include_directories(system-metrics
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/datatypes>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/decoder>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/log-components>
    )

set_target_properties(system-metrics PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(system-metrics PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})
