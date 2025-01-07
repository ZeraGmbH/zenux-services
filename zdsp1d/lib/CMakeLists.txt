file(GLOB HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/device-nodes/*.h
)

file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/device-nodes/*.cpp
)

add_library(zdsp1d-lib STATIC
    ${HEADERS}
    ${SOURCES}
    )

target_include_directories(zdsp1d-lib
    PUBLIC
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/zdsp1d-lib>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/device-nodes>
    )

if(${Systemd_FOUND})
    target_link_libraries(zdsp1d-lib
        PRIVATE
        systemd
    )
endif()

target_link_libraries(zdsp1d-lib
    PUBLIC
    Qt5::Xml
    Qt5::Network
    Qt5::Core
    Zera::xiqnet
    Zera::zeraprotobuf
    Zera::zera-log-statistics
    zenuxservicecommon
    )

set_target_properties(zdsp1d-lib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(zdsp1d-lib PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

export(TARGETS zdsp1d-lib NAMESPACE Zera:: FILE ZenuxServicesTargets.cmake)
install(TARGETS zdsp1d-lib
    EXPORT ZenuxServicesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(
    FILES ${HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zdsp1d-lib
    )
