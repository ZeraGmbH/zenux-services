file(GLOB SCRIPTS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.py
)

add_custom_target(
    python-script-tests
    SOURCES ${SCRIPTS}
)

add_subdirectory(data)

