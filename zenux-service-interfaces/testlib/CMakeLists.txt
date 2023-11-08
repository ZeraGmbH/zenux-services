file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

add_library(service-interfaces-testlib STATIC
    ${SOURCES}
    ${HEADER}
)

target_include_directories(service-interfaces-testlib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/service-interfaces-testlib>
    )

target_link_libraries(service-interfaces-testlib
    PUBLIC
    service-interfaces
    Zera::zera-timers-testlib
)

generate_export_header(service-interfaces-testlib)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/service-interfaces-testlib_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/service-interfaces-testlib
    )

export(TARGETS service-interfaces-testlib NAMESPACE Zera:: FILE ZenuxServicesTargets.cmake)
install(TARGETS service-interfaces-testlib
    EXPORT ZenuxServicesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/service-interfaces-testlib
    )
