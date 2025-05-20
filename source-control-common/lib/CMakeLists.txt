file(GLOB HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
)

file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

file(GLOB SOURCE_DEVINFO
    ${CMAKE_CURRENT_SOURCE_DIR}/deviceinfo/*.json
)

# Note 1: In OE we do not (yet) have jsonlint-php so it must be optional
# Note 2: In case we make COMMAND in target below conditional on JSONLINT,
#         qt-creator does not display source-capabilities. That's what the
#         'echo' dance is for
find_program(JSONLINT NAMES jsonlint-php echo)
if(${JSONLINT} MATCHES ".*echo.*")
    message(WARNING "Program jsonlint-php not found. On Fedora install php-jsonlint")
endif()

# check for valid devinfo / display in qt-creator
add_custom_target(source-deviceinfo-dont-forget-qrc ALL
    SOURCES ${SOURCE_DEVINFO}
    COMMAND ${JSONLINT} ${SOURCE_DEVINFO}
    )

# check if all configs made it into resource.qrc
foreach(sourceconfigfile ${SOURCE_DEVINFO})
    get_filename_component(filename ${sourceconfigfile} NAME)
    file(STRINGS ${CMAKE_CURRENT_SOURCE_DIR}/resource.qrc lines REGEX "${filename}")
    if(NOT lines)
        message(FATAL_ERROR "${filename} was not added to resource.qrc - source will not work!!")
    endif()
endforeach()

add_library(source-control-common-lib SHARED
    ${HEADERS}
    ${SOURCES}
    resource.qrc
    )

target_include_directories(source-control-common-lib
    PUBLIC
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/source-control-common-lib>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(source-control-common-lib
    PUBLIC
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    zera-json-params
    )

set_target_properties(source-control-common-lib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(source-control-common-lib PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

add_definitions(-DZC_DEV_STATE_PATH="${CMAKE_INSTALL_SYSCONFDIR}/zera/states")

export(TARGETS source-control-common-lib NAMESPACE Zera:: FILE ZenuxServicesTargets.cmake)
install(TARGETS source-control-common-lib
    EXPORT ZenuxServicesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(
    FILES ${HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/source-control-common-lib
    )
