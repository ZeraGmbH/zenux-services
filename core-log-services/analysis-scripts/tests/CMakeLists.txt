#####################################################################
#                                                                   #
#   IMPORTANT NOTE                                                  #
# By Default these tests will not show up in Qt Creator             #
# Either enable them in project -> testing -> check CTest checkbox  #
# or run them locally by "./test_logAnalysis.py"                    #
#                                                                   #
#####################################################################

add_test (NAME analysis-script-tests
  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/test_logAnalysis.py
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

file(GLOB TESTS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.py
    ${CMAKE_CURRENT_SOURCE_DIR}/data/*
)

add_custom_target(
    python-script-tests
    SOURCES ${TESTS}
)
