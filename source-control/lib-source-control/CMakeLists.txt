file(GLOB HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
)

file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

add_library(source-control-lib SHARED
    ${HEADERS}
    ${SOURCES}
    )

target_include_directories(source-control-lib
    PUBLIC
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/source-control-lib>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(source-control-lib
    PUBLIC
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    zenuxservicecommon
    zera-json-params
    Zera::zera-tasks-base
    )

set_target_properties(source-control-lib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(source-control-lib PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

export(TARGETS source-control-lib NAMESPACE Zera:: FILE ZenuxServicesTargets.cmake)
install(TARGETS source-control-lib
    EXPORT ZenuxServicesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(
    FILES ${HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/source-control-lib
    )
