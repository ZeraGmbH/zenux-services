function(SETUP_MT310S2_TESTS)
    foreach(_testname ${ARGN})
        SETUP_TESTS(${_testname})
        target_link_libraries(${_testname}
            PUBLIC
            mt310s2d-testlib
            Zera::xml-compare-testlib
        )
    endforeach()
endfunction()

SETUP_MT310S2_TESTS(
    test_config_load_mt310s2
    test_emob_lock_button
    test_regression_sense_range_mt310s2
    test_regression_sense_interface_mt310s2
    test_regression_sense_interface_mt581s2
    test_regression_adj_import_export_xml_mt310s2
    test_regression_adj_import_export_eeprom_mt310s2
    test_regression_adj_import_permission_mt310s2
    test_regression_adj_scpi_query_format_mt310s2
    test_regression_adj_calc_mt310s2
    test_adj_deny_import_mt310s2
    test_regression_critical_status_mt310s2
    test_regression_adj_status_mt310s2
    test_regression_scpi_mt310s2
    )

target_sources(test_regression_adj_import_export_xml_mt310s2
  PRIVATE
    regression_data_adj_import_export/regression_data.qrc
)

target_sources(test_regression_adj_import_export_eeprom_mt310s2
  PRIVATE
    regression_data_adj_import_export/regression_data.qrc
)

target_sources(test_regression_adj_import_permission_mt310s2
  PRIVATE
    regression_data_adj_import_export/regression_data.qrc
)

target_sources(test_regression_sense_interface_mt310s2
  PRIVATE
    regression_data_sense_constants/regression_data.qrc
)

target_sources(test_regression_sense_interface_mt581s2
  PRIVATE
    regression_data_sense_constants/regression_data.qrc
)

target_sources(test_regression_adj_scpi_query_format_mt310s2
  PRIVATE
    regression_data_adj_scpi_format/regression_data.qrc
)

target_sources(test_regression_adj_calc_mt310s2
  PRIVATE
    regression_data_adj_calc/regression_data.qrc
)

target_sources(test_adj_deny_import_mt310s2
  PRIVATE
    regression_data_adj_import_export/regression_data.qrc
)

target_sources(test_regression_scpi_mt310s2
    PRIVATE
    test-data/regression_data.qrc
)
