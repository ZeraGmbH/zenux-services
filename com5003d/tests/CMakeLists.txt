function(SETUP_COM5003_TESTS)
    foreach(_testname ${ARGN})
        SETUP_TESTS(${_testname})
        target_link_libraries(${_testname}
            PUBLIC
            com5003d-lib
            com5003-testlib
            Zera::xml-compare-testlib
        )
    endforeach()
endfunction()

SETUP_COM5003_TESTS(
    test_config_load_com5003
    test_regression_sense_range_com5003
    test_regression_sense_interface_com5003
    test_regression_adj_import_export_xml_com5003
    test_regression_adj_import_export_eeprom_com5003
    test_regression_adj_import_permission_com5003
    test_regression_adj_scpi_query_format_com5003
    test_regression_adj_calc_com5003
    test_regression_critical_status_com5003
    test_adj_deny_import_com5003
    test_regression_adj_status_com5003
    test_regression_scpi_com5003
    )

target_sources(test_regression_adj_import_export_xml_com5003
  PRIVATE
    regression_data_adj_import_export/regression_data.qrc
)

target_sources(test_regression_adj_import_export_eeprom_com5003
  PRIVATE
    regression_data_adj_import_export/regression_data.qrc
)

target_sources(test_regression_adj_import_permission_com5003
  PRIVATE
    regression_data_adj_import_export/regression_data.qrc
)

target_sources(test_regression_sense_interface_com5003
  PRIVATE
    regression_data_sense_constants/regression_data.qrc
)

target_sources(test_regression_adj_scpi_query_format_com5003
  PRIVATE
    regression_data_adj_scpi_format/regression_data.qrc
)

target_sources(test_regression_adj_calc_com5003
  PRIVATE
    regression_data_adj_calc/regression_data.qrc
)

target_sources(test_adj_deny_import_com5003
  PRIVATE
    regression_data_adj_import_export/regression_data.qrc
)

target_sources(test_regression_scpi_com5003
    PRIVATE
    test-data/regression_data.qrc
)
