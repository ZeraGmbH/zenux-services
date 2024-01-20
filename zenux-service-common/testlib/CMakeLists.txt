file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/micro-controller-io/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/micro-controller-io/test-specific/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/micro-controller-io/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/micro-controller-io/test-specific/*.h
    )

add_library(zenuxservicecommon-testlib STATIC
    ${SOURCES}
    ${HEADER}
)

target_include_directories(zenuxservicecommon-testlib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/micro-controller-io>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/micro-controller-io/test-specific>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/zenuxservicecommon-testlib>
    )

target_link_libraries(zenuxservicecommon-testlib
    PUBLIC
    Qt5::Core
    Qt5::Test
    zenuxservicecommon
    zenuxservicecommon-demolib
    service-interfaces
    Zera::zera-timers-testlib
)

set_target_properties(zenuxservicecommon-testlib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(zenuxservicecommon-testlib PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

generate_export_header(zenuxservicecommon-testlib)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/zenuxservicecommon-testlib_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zenuxservicecommon-testlib
    )

export(TARGETS zenuxservicecommon-testlib NAMESPACE Zera:: FILE ZenuxServicesTargets.cmake)
install(TARGETS zenuxservicecommon-testlib
    EXPORT ZenuxServicesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zenuxservicecommon-testlib
    )
