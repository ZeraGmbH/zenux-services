file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/log-components/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/log-components/*.h
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
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/dsp-helper>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/log-components>
    )

set_target_properties(system-metrics PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(system-metrics PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})
