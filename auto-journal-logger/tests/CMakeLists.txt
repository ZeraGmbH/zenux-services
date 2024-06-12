function(SETUP_AUTO_JOURNAL_LOGGER_TESTS)
    foreach(_testname ${ARGN})
        SETUP_TESTS(${_testname})
        target_link_libraries(${_testname}
            PUBLIC
            auto-journal-logger-lib
            auto-journal-logger-testlib
        )
    endforeach()
endfunction()

SETUP_AUTO_JOURNAL_LOGGER_TESTS(
    test_onofffilewatcher
    test_periodic_log_generator
    test_fileutils
)
