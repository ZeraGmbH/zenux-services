file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/device-nodes/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/device-nodes/*.h
    )

add_library(sec1000d-demolib STATIC
    ${SOURCES}
    ${HEADER}
)

target_include_directories(sec1000d-demolib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/device-nodes>
    )

target_link_libraries(sec1000d-demolib
    PUBLIC
    sec1000d-lib
)

set_target_properties(sec1000d-demolib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(sec1000d-demolib PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

export(TARGETS sec1000d-demolib NAMESPACE Zera:: FILE ZenuxServicesTargets.cmake)
install(TARGETS sec1000d-demolib
    EXPORT ZenuxServicesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/sec1000d-demolib
    )
