file(GLOB SOURCES
    *.cpp
    )

file(GLOB HEADER
    *.h
    )

add_library(auto-journal-logger-lib SHARED
    ${HEADER}
    ${SOURCES}
)

target_include_directories(auto-journal-logger-lib
    PUBLIC
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/auto-journal-logger-lib>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(auto-journal-logger-lib
    PUBLIC
    ${QTX}::Core
    PRIVATE
    Zera::zera-timers
    simplecmdiobase
    )

set_target_properties(auto-journal-logger-lib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(auto-journal-logger-lib PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

export(TARGETS auto-journal-logger-lib NAMESPACE Zera:: FILE ZenuxServicesTargets.cmake)
install(TARGETS auto-journal-logger-lib
    EXPORT ZenuxServicesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/auto-journal-logger-lib
    )

