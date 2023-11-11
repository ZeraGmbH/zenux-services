file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

add_library(zdsp1d-testlib STATIC
    ${SOURCES}
    ${HEADER}
)

target_include_directories(zdsp1d-testlib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(zdsp1d-testlib
    PUBLIC
    zdsp1d-lib
)

set_target_properties(zdsp1d-testlib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(zdsp1d-testlib PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})
