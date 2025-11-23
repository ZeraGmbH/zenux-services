file(GLOB SOURCES
    *.cpp
    )

file(GLOB HEADER
    *.h
    )

add_library(zenuxi2ceeprom-demolib STATIC
    ${SOURCES}
    ${HEADER}
)

target_include_directories(zenuxi2ceeprom-demolib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/zenuxi2ceeprom-demolib>
    )

target_link_libraries(zenuxi2ceeprom-demolib
    PUBLIC
    zenuxi2ceeprom
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
)

set_target_properties(zenuxi2ceeprom-demolib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(zenuxi2ceeprom-demolib PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

generate_export_header(zenuxi2ceeprom-demolib)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/zenuxi2ceeprom-demolib_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zenuxi2ceeprom-demolib
    )

export(TARGETS zenuxi2ceeprom-demolib NAMESPACE Zera:: FILE ZenuxServicesTargets.cmake)
install(TARGETS zenuxi2ceeprom-demolib
    EXPORT ZenuxServicesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zenuxi2ceeprom-demolib
    )
