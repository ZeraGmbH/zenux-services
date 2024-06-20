function(SETUP_AUTO_JOURNAL_LOGGER_TESTS)
    foreach(_testname ${ARGN})
        SETUP_TESTS(${_testname})
        target_link_libraries(${_testname}
            PUBLIC
            auto-journal-logger-lib
        )
    endforeach()
endfunction()

SETUP_AUTO_JOURNAL_LOGGER_TESTS(
    test_fileutils
    test_cmd_handler
)
