function(SETUP_SEC1000_TESTS)
    foreach(_testname ${ARGN})
        SETUP_TESTS(${_testname})
        target_link_libraries(${_testname}
            PUBLIC
            zenuxservicecommon-demolib
            zenuxservicecommon-testlib
            Zera::xml-compare-testlib
            sec1000d-demolib
        )
    endforeach()
endfunction()

SETUP_SEC1000_TESTS(
    test_config_load_sec1000
    test_sec_resource
    test_regression_sec1000
    )

target_sources(test_regression_sec1000
    PRIVATE
    test-data/regression_data.qrc
)
