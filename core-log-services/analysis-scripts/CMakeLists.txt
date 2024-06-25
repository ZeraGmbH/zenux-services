file(GLOB SCRIPTS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.py
)

add_custom_target(
    python-scripts
    SOURCES ${SCRIPTS}
)

add_test (NAME analysis-script-tests
  COMMAND ${Python_EXECUTABLE} tests/test_logAnalysis.py
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_subdirectory(tests)
