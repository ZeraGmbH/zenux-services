function(SETUP_SEC1000_TESTS)
    foreach(_testname ${ARGN})
        SETUP_TESTS(${_testname})
        target_link_libraries(${_testname}
            PUBLIC
            sec1000d-demolib
        )
    endforeach()
endfunction()

SETUP_SEC1000_TESTS(
    test_config_load_sec1000
    test_sec_resource
    )
