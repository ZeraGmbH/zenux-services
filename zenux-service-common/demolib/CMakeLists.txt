file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/device-nodes/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/micro-controller-io/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/device-nodes/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/micro-controller-io/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

file(GLOB RESOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/../../*/configs/*.qrc
    )

if(GUI_SIMULATION)
    file(GLOB SOURCES_SIMUL_GUI ${CMAKE_CURRENT_SOURCE_DIR}/simul-gui-lib/*.cpp)
    file(GLOB HEADER_SIMUL_GUI ${CMAKE_CURRENT_SOURCE_DIR}/simul-gui-lib/*.h)
    file(GLOB RESOURCES_SIMUL_GUI ${CMAKE_CURRENT_SOURCE_DIR}/simul-gui-lib/*.qrc)
endif()

add_library(zenuxservicecommon-demolib STATIC
    ${SOURCES} ${SOURCES_SIMUL_GUI}
    ${HEADER} ${HEADER_SIMUL_GUI}
    ${RESOURCES} ${RESOURCES_SIMUL_GUI}
)

target_include_directories(zenuxservicecommon-demolib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/device-nodes>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/micro-controller-io>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/simul-gui-lib>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/zenuxservicecommon-demolib>
    )

target_link_libraries(zenuxservicecommon-demolib
    PUBLIC
    Qt5::Core
    zenuxservicecommon
    zenuxi2ceeprom-demolib
    mt310s2d-lib
    com5003d-lib
    sec1000d-lib
    sec1000d-demolib
    zdsp1d-lib
    zdsp1d-demolib
    Zera::zera-timers-demolib
    Zera::resman-lib
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    PRIVATE
    auto-journal-logger-lib
)

if(GUI_SIMULATION)
    target_compile_definitions(zenuxservicecommon-demolib PUBLIC $<$<CONFIG:Debug>:QT_QML_DEBUG>)
    target_compile_definitions(zenuxservicecommon-demolib PUBLIC $<$<CONFIG:Debug>:QT_DECLARATIVE_DEBUG>)
    target_link_libraries(zenuxservicecommon-demolib
        PUBLIC
        Qt5::Qml
        Qt5::Quick
    )
endif()

set_target_properties(zenuxservicecommon-demolib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(zenuxservicecommon-demolib PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

generate_export_header(zenuxservicecommon-demolib)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/zenuxservicecommon-demolib_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zenuxservicecommon-demolib
    )

export(TARGETS zenuxservicecommon-demolib NAMESPACE Zera:: FILE ZenuxServicesTargets.cmake)
install(TARGETS zenuxservicecommon-demolib
    EXPORT ZenuxServicesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zenuxservicecommon-demolib
    )
