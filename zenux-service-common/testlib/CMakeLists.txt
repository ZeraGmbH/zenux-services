file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

add_library(zenuxservicecommon-testlib STATIC
    ${SOURCES}
    ${HEADER}
)

target_include_directories(zenuxservicecommon-testlib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(zenuxservicecommon-testlib
    PUBLIC
    Qt5::Core
    zenuxservicecommon
)
