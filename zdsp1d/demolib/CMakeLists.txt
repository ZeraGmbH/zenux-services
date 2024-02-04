file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/device-nodes/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/device-nodes/*.h
    )

add_library(zdsp1d-demolib STATIC
    ${SOURCES}
    ${HEADER}
)

target_include_directories(zdsp1d-demolib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/device-nodes>
    )

target_link_libraries(zdsp1d-demolib
    PUBLIC
    zdsp1d-lib
)

set_target_properties(zdsp1d-demolib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(zdsp1d-demolib PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

export(TARGETS zdsp1d-demolib NAMESPACE Zera:: FILE ZenuxServicesTargets.cmake)
install(TARGETS zdsp1d-demolib
    EXPORT ZenuxServicesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zdsp1d-demolib
    )
