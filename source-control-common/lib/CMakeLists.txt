file(GLOB HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
)

file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)


add_library(source-control-common-lib SHARED
    ${HEADERS}
    ${SOURCES}
    )

target_include_directories(source-control-common-lib
    PUBLIC
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/source-control-common-lib>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(source-control-common-lib
    PUBLIC
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    zera-json-params
    )

add_definitions(-DZC_DEV_STATE_PATH="${CMAKE_INSTALL_SYSCONFDIR}/zera/states")

set_target_properties(source-control-common-lib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(source-control-common-lib PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

export(TARGETS source-control-common-lib NAMESPACE Zera:: FILE ZenuxServicesTargets.cmake)
install(TARGETS source-control-common-lib
    EXPORT ZenuxServicesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(
    FILES ${HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/source-control-common-lib
    )
