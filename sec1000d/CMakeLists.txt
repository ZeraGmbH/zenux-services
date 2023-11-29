set(SOURCES
    sec1000d.cpp
    statusinterface.cpp
    systeminterface.cpp
    systeminfo.cpp
    )

set(HEADER
    sec1000d.h
    statusinterface.h
    systeminterface.h
    systeminfo.h
    )

add_library(sec1000d-lib STATIC
    ${HEADER}
    ${SOURCES}
    )

target_include_directories(sec1000d-lib
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    )

if(${Systemd_FOUND})
    target_link_libraries(sec1000d-lib
        PRIVATE
        systemd
    )
endif()

target_link_libraries(sec1000d-lib
    PUBLIC
    Qt5::Xml
    Qt5::Network
    Zera::zera-xmlconfig
    Zera::zeraprotobuf
    zenuxservicecommon
    )

add_executable(sec1000d
    main.cpp
    )

target_link_libraries(sec1000d
    PUBLIC
    sec1000d-lib
    )

install(TARGETS sec1000d
    EXPORT sec1000dExport
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

add_subdirectory(configs)
add_subdirectory(systemd)
add_subdirectory(tests)
