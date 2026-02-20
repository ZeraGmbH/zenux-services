file(GLOB HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/device-nodes/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/sanity-checks/*.h
)

file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/device-nodes/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sanity-checks/*.cpp
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
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sanity-checks>
    )

if(${Systemd_FOUND})
    target_link_libraries(zdsp1d-lib
        PRIVATE
        systemd
    )
endif()

target_link_libraries(zdsp1d-lib
    PUBLIC
    ${QTX}::Xml
    ${QTX}::Network
    ${QTX}::Core
    Zera::xiqnet
    Zera::zeraprotobuf
    Zera::zera-log-statistics
    zenuxservicecommon
    )

if("${QT_MAJOR_VER}" STREQUAL "6")
    target_link_libraries(zdsp1d-lib
        PUBLIC
        ${QTX}::StateMachine
    )
endif()

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
