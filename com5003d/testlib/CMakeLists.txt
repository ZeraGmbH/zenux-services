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
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/com5003-testlib>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(com5003-testlib
    PUBLIC
    com5003d-lib
    zenuxservicecommon-demolib
    zenuxservicecommon-testlib
)

export(TARGETS com5003-testlib NAMESPACE Zera:: FILE ZenuxServicesTargets.cmake)
install(TARGETS com5003-testlib
    EXPORT ZenuxServicesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/com5003-testlib
    )
