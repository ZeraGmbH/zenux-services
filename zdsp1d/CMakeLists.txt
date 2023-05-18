file(GLOB SOURCES
    *.cpp 
    )


file(GLOB HEADER
    *.h  
    )

add_library(zdsp1d-lib STATIC
    ${HEADER}
    ${SOURCES}
    )

target_include_directories(zdsp1d-lib
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
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
    VeinMeta::xiqnet
    VeinMeta::zeraprotobuf
    Zera::zera-xmlconfig
    zenuxservicecommon
    )

add_executable(zdsp1d
    main.cpp
    )

target_link_libraries(zdsp1d
    PUBLIC
    zdsp1d-lib
    )

install(TARGETS zdsp1d
    EXPORT zdsp1dExport
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

if(${useSystemd})
    set(SYSTEMD_UNITS
        systemd/zdsp1d.service
        systemd/zdsp1d.socket
        )
    add_custom_target(zdsp1d-systemd-units ALL
        DEPENDS ${SYSTEMD_UNITS}
        SOURCES ${SYSTEMD_UNITS}
        COMMAND echo "Install systemd units..."
        )
    install(
        FILES ${SYSTEMD_UNITS}
        DESTINATION ${SYSTEMD_UNIT_INSTALL_PATH}
        )
endif()

add_subdirectory(configs)
add_subdirectory(tests)
