SETUP_TESTS(
    test_accumulatorinterface
    test_config_load_mt310s2
    test_regression_sense_range_mt310s2
    test_regression_sense_interface_mt310s2
    test_regression_adj_import_export_xml_mt310s2
    test_regression_adj_import_export_eeprom_mt310s2
    test_regression_adj_import_permission_mt310s2
    test_regression_adj_scpi_query_format_mt310s2
    test_regression_adj_calc_mt310s2
    test_adj_deny_import_mt310s2
    test_accumulatorinterface_mock
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

target_sources(test_accumulatorinterface_mock
  PRIVATE
    regression_data_accumulator/regression_data.qrc
)
