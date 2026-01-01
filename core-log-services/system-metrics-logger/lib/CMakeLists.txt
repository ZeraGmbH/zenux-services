file(GLOB SOURCES
    *.cpp
    decoder/*.cpp
    log-collectors/*.cpp
    log-getters/*.cpp
    log-strategies/*.cpp
    )

file(GLOB HEADER
    *.h
    datatypes/*.h
    decoder/*.h
    log-collectors/*.h
    log-getters/*.h
    log-strategies/*.h
    )

add_library(system-metrics STATIC
    ${SOURCES}
    ${HEADER}
)

target_link_libraries(system-metrics
    PUBLIC
    ${QTX}::Core
    Zera::zera-timers
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

target_include_directories(system-metrics
    PUBLIC
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/system-metrics>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/datatypes>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/decoder>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/log-collectors>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/log-getters>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/log-strategies>
    )

set_target_properties(system-metrics PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(system-metrics PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

install(TARGETS system-metrics
    EXPORT ZenuxServicesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

export(TARGETS system-metrics FILE ZenuxServicesTargets.cmake)
install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/system-metrics
    )
