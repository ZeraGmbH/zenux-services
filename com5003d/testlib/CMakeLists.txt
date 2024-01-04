file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

add_library(com5003-testlib STATIC
    ${SOURCES}
    ${HEADER}
)

target_include_directories(com5003-testlib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(com5003-testlib
    PUBLIC
    com5003d-lib
    zenuxservicecommon-demolib
    zenuxservicecommon-testlib
)
