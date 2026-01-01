file(GLOB SOURCES
    *.cpp
    )

file(GLOB HEADER
    *.h
    )

file(GLOB_RECURSE RESOURCES
    *.qrc
    )

add_library(mt310s2d-testlib STATIC
    ${SOURCES}
    ${HEADER}
    ${RESOURCES}
)

target_include_directories(mt310s2d-testlib
    PUBLIC
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/mt310s2d-testlib>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(mt310s2d-testlib
    PUBLIC
    ${QTX}::Core
    mt310s2d-lib
    zenuxservicecommon-demolib
    zenuxservicecommon-testlib
    Zera::testlib-common
)

export(TARGETS mt310s2d-testlib NAMESPACE Zera:: FILE ZenuxServicesTargets.cmake)
install(TARGETS mt310s2d-testlib
    EXPORT ZenuxServicesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mt310s2d-testlib
    )
