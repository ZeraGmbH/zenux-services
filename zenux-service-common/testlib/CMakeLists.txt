file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/micro-controller-io/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/micro-controller-io/*.h
    )

add_library(zenuxservicecommon-testlib STATIC
    ${SOURCES}
    ${HEADER}
)

target_include_directories(zenuxservicecommon-testlib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/micro-controller-io>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
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
