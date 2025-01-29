function(SETUP_AUTO_JOURNAL_LOGGER_TESTS)
    foreach(_testname ${ARGN})
        SETUP_TESTS(${_testname})
        target_link_libraries(${_testname}
            PUBLIC
            auto-journal-logger-lib
            auto-journal-logger-testlib
            simplecmdio-client
        )
    endforeach()
endfunction()

SETUP_AUTO_JOURNAL_LOGGER_TESTS(
    test_fileutils
    test_cmd_handler
    test_auto_journal_logger
)

target_sources(test_auto_journal_logger
    PRIVATE
    test-data/regression_data.qrc
)
