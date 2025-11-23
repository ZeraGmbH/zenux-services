file(GLOB HEADERS
    *.h
    )

file(GLOB SOURCES
    *.cpp
    )

file(GLOB RESOURCES
)

add_library(zenuxi2ceeprom SHARED
    ${HEADERS}
    ${SOURCES}
)

target_link_libraries(zenuxi2ceeprom
    PUBLIC
    Zera::zera-i2c-devices
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
)

target_include_directories(zenuxi2ceeprom
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/zenuxi2ceeprom>
    )

set_target_properties(zenuxi2ceeprom PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(zenuxi2ceeprom PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

install(TARGETS zenuxi2ceeprom
    EXPORT ZenuxServicesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

generate_export_header(zenuxi2ceeprom)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/zenuxi2ceeprom_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zenuxi2ceeprom
    )

export(TARGETS zenuxi2ceeprom FILE ZenuxServicesTargets.cmake)
install(
    FILES ${HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zenuxi2ceeprom
    )

