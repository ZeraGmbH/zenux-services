function(SETUP_COREDUMP_COLLECTOR_TESTS)
    foreach(_testname ${ARGN})
        SETUP_TESTS(${_testname})
        target_link_libraries(${_testname}
            PUBLIC
            coredump-collector-lib
        )
    endforeach()
endfunction()

SETUP_COREDUMP_COLLECTOR_TESTS (
    test_coredump_watcher
)

