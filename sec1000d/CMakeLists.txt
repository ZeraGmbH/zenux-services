set(SOURCES
    resources/secgroupresourceandinterface.cpp
    scpi-interfaces/secchannel.cpp
    settings/seccalcsettings.cpp
    settings/secinputsettings.cpp
    sec1000d.cpp
    statusinterface.cpp
    systeminterface.cpp
    systeminfo.cpp
    )

set(HEADER
    resources/secgroupresourceandinterface.h
    scpi-interfaces/secchannel.h
    settings/seccalcsettings.h
    settings/secinputsettings.h
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
    PUBLIC
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/sec1000d-lib>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/resources>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/scpi-interfaces>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/settings>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
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

set_target_properties(sec1000d-lib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(sec1000d-lib PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

export(TARGETS sec1000d-lib NAMESPACE Zera:: FILE ZenuxServicesTargets.cmake)
install(TARGETS sec1000d-lib
    EXPORT ZenuxServicesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/sec1000d-lib
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
