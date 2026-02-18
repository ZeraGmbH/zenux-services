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
            Zera::xml-compare-testlib
            Zera::testlib-common
        )
    endforeach()
endfunction()

SETUP_SERVICE_ZDSP1D_TESTS(
    test_config_load_zdsp1
    test_regression_dsp_var
    test_regression_scpi_zdsp1d
    test_zdspclientcontainer
    )

target_sources(test_regression_scpi_zdsp1d
    PRIVATE
    test-data/regression_data.qrc
)

target_sources(test_regression_dsp_var
    PRIVATE
    test-data/regression_data.qrc
)
