file(GLOB HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
)

file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)


add_library(source-json-io-lib SHARED
    ${HEADERS}
    ${SOURCES}
    )

target_include_directories(source-json-io-lib
    PUBLIC
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/source-json-io-lib>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(source-json-io-lib
    PUBLIC
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    zera-json-params
    )

set_target_properties(source-json-io-lib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(source-json-io-lib PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

export(TARGETS source-json-io-lib NAMESPACE Zera:: FILE ZenuxServicesTargets.cmake)
install(TARGETS source-json-io-lib
    EXPORT ZenuxServicesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(
    FILES ${HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/source-json-io-lib
    )
