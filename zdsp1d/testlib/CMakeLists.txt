file(GLOB SOURCES
    *.cpp
    device-nodes/*.cpp
    )

file(GLOB HEADER
    *.h
    device-nodes/*.h
    )

add_library(zdsp1d-testlib STATIC
    ${SOURCES}
    ${HEADER}
)

target_include_directories(zdsp1d-testlib
    PUBLIC
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/zdsp1d-testlib>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/device-nodes>
    )

target_link_libraries(zdsp1d-testlib
    PUBLIC
    zenuxservicecommon-demolib
    zdsp1d-lib
)

set_target_properties(zdsp1d-testlib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(zdsp1d-testlib PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

export(TARGETS zdsp1d-testlib NAMESPACE Zera:: FILE ZenuxServicesTargets.cmake)
install(TARGETS zdsp1d-testlib
    EXPORT ZenuxServicesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zdsp1d-testlib
    )
