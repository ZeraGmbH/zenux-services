file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

add_library(service-interfaces-demolib STATIC
    ${SOURCES}
    ${HEADER}
)

target_include_directories(service-interfaces-demolib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/service-interfaces-demolib>
    )

target_link_libraries(service-interfaces-demolib
    PUBLIC
    service-interfaces
)

generate_export_header(service-interfaces-demolib)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/service-interfaces-demolib_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/service-interfaces-demolib
    )

export(TARGETS service-interfaces-demolib NAMESPACE Zera:: FILE ZenuxServicesTargets.cmake)
install(TARGETS service-interfaces-demolib
    EXPORT ZenuxServicesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/service-interfaces-demolib
    )
