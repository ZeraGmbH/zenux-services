file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

add_library(mt310s2d-lib-testlib STATIC
    ${SOURCES}
    ${HEADER}
)

target_include_directories(mt310s2d-lib-testlib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(mt310s2d-lib-testlib
    PUBLIC
    Qt5::Core
    mt310s2d-lib
    zenuxservicecommon-demolib
)
