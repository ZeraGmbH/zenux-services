file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/device-nodes/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/device-nodes/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

file(GLOB RESOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/../../*/configs/*.qrc
    )

add_library(zenuxservicecommon-demolib STATIC
    ${SOURCES}
    ${HEADER}
    ${RESOURCES}
)

target_include_directories(zenuxservicecommon-demolib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/device-nodes>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/zenuxservicecommon-demolib>
    )

target_link_libraries(zenuxservicecommon-demolib
    PUBLIC
    Qt5::Core
    zenuxservicecommon
    zdsp1d-lib
)

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
