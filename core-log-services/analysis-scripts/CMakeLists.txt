file(GLOB SCRIPTS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.py
)

add_custom_target(
    python-scripts
    SOURCES ${SCRIPTS}
)
