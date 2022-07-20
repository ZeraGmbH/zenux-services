file(GLOB SOURCES
    *.cpp 
    )


file(GLOB HEADER
    *.h  
    )

set(OTHER_FILES 
    zdsp1d.xsd
    zdsp1d.xml
)

add_executable(zdsp1d
    ${SOURCES}
    ${HEADER}
    )

target_link_libraries(zdsp1d
    PUBLIC
    Qt5::Xml
    Qt5::Network
    Qt5::Core
    VeinMeta::xiqnet
    VeinMeta::zeraprotobuf
    ZeraClasses::zeraxmlconfig
    zeraservercommon
    )

if(${Systemd_FOUND})
    target_link_libraries(zdsp1d
        PRIVATE
        systemd
    )
endif()

target_include_directories(zdsp1d
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    )

install(TARGETS zdsp1d
    EXPORT zdsp1dExport
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

install(
    FILES ${OTHER_FILES}
    DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/zera/zdsp1d
    )

