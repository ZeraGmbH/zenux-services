file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

add_library(mt310s2d-testlib STATIC
    ${SOURCES}
    ${HEADER}
)

target_include_directories(mt310s2d-testlib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(mt310s2d-testlib
    PUBLIC
    Qt5::Core
    mt310s2d-lib
    zenuxservicecommon-demolib
    zenuxservicecommon-testlib
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
