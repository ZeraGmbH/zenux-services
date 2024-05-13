function(SETUP_SERVICE_ZDSP1D_TESTS)
    foreach(_testname ${ARGN})
        SETUP_TESTS(${_testname})
        target_link_libraries(${_testname}
            PUBLIC
            zenuxservicecommon-demolib
            zenuxservicecommon-testlib
            service-interfaces-testlib
            zdsp1d-demolib
            zdsp1d-testlib
        )
    endforeach()
endfunction()

SETUP_SERVICE_ZDSP1D_TESTS(
    test_config_load_zdsp1
    test_old_scpi_implementation
    test_regression_dsp_var
    )

