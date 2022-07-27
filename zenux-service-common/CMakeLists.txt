set(HEADER
    notzeronumgen.h
    meas-channels/justnode.h
    micro-controller-io/atmelwatcher.h
    resources/resource.h
    resources/rmconnection.h
    settings/fpgasettings.h
    )

add_library(zenuxservicecommon SHARED
    ${HEADER}
    notzeronumgen.cpp
    meas-channels/justnode.cpp
    micro-controller-io/atmelwatcher.cpp
    resources/resource.cpp
    resources/rmconnection.cpp
    settings/fpgasettings.cpp
    )

target_link_libraries(zenuxservicecommon
    PUBLIC
    Qt5::Core
    VeinMeta::xiqnet
    Zera::SCPI
    Zera::scpi-tcp
    ZeraClasses::zeraxmlconfig
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

# announce headers - target perspective
target_include_directories(zenuxservicecommon
    PUBLIC
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/zenuxservicecommon>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/meas-channels>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/micro-controller-io>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/resources>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/settings>
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

# configure *ConfigVersion.cmake
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    zenuxservicecommonConfigVersion.cmake
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY SameMajorVersion
    )

# configure *Config.cmake
configure_file(zenuxservicecommonConfig.cmake.in zenuxservicecommonConfig.cmake @ONLY)

# install *Config(Version).cmake
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/zenuxservicecommonConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/zenuxservicecommonConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/zenuxservicecommon
    )

# install targets cmake-files
install(EXPORT zenuxservicecommonExport
    FILE zenuxservicecommonTargets.cmake
    NAMESPACE ZeraClasses::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/zenuxservicecommon
    )

# install public headers
install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zenuxservicecommon
    )
