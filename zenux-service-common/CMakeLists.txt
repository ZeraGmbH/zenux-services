set(HEADER
    src/justdata.h
    src/notzeronumgen.h
    src/meas-channels/justnode.h
    src/micro-controller-io/atmelerrorlog.h
    src/micro-controller-io/atmelwatcher.h
    src/resources/resource.h
    src/resources/rmconnection.h
    src/settings/debugsettings.h
    src/settings/fpgasettings.h
    )

add_library(zenuxservicecommon SHARED
    ${HEADER}
    src/justdata.cpp
    src/notzeronumgen.cpp
    src/meas-channels/justnode.cpp
    src/micro-controller-io/atmelerrorlog.cpp
    src/micro-controller-io/atmelwatcher.cpp
    src/resources/resource.cpp
    src/resources/rmconnection.cpp
    src/settings/debugsettings.cpp
    src/settings/fpgasettings.cpp
    )

target_link_libraries(zenuxservicecommon
    PUBLIC
    Qt5::Core
    VeinMeta::xiqnet
    Zera::SCPI
    Zera::scpi-tcp
    Zera::zera-xmlconfig
    Zera::zera-math
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

# announce headers - target perspective
target_include_directories(zenuxservicecommon
    PUBLIC
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/zenuxservicecommon>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/meas-channels>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/micro-controller-io>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/resources>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/settings>
    )

set_target_properties(zenuxservicecommon PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(zenuxservicecommon PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

generate_export_header(zenuxservicecommon)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/zenuxservicecommon_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zenuxservicecommon
    )

install(TARGETS zenuxservicecommon
    EXPORT zenuxservicecommonExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

add_subdirectory(tests)
