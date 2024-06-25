file(GLOB SCRIPTS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.py
)

add_custom_target(
    python-scripts
    SOURCES ${SCRIPTS}
)

#####################################################################
#                                                                   #
#   IMPORTANT NOTE                                                  #
# By Default these tests will not show up in Qt Creator             #
# Either enable them in project -> testing -> check CTest checkbox  #
# or run them locally by "python3 tests/test_logAnalysis.py"        #
#                                                                   #
#####################################################################

add_test (NAME analysis-script-tests
  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_logAnalysis.py
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_subdirectory(tests)
