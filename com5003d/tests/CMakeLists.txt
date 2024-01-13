SETUP_TESTS(
    test_config_load_com5003
    test_sense_regression_range_com5003
    test_sense_regression_interface_com5003
    test_adj_regression_import_export_xml_com5003
    test_adj_regression_import_export_eeprom_com5003
    test_adj_regression_import_permission_com5003
    test_adj_regression_scpi_query_format_com5003
    test_adj_regression_calc_com5003
    test_adj_deny_import_com5003
    test_adj_import_invalid_eeprom_com5003
    )

target_sources(test_adj_regression_import_export_xml_com5003
  PRIVATE
    regression_data_adj_import_export/regression_data.qrc
)

target_sources(test_adj_regression_import_export_eeprom_com5003
  PRIVATE
    regression_data_adj_import_export/regression_data.qrc
)

target_sources(test_adj_regression_import_permission_com5003
  PRIVATE
    regression_data_adj_import_export/regression_data.qrc
)

target_sources(test_sense_regression_interface_com5003
  PRIVATE
    regression_data_sense_constants/regression_data.qrc
)

target_sources(test_adj_regression_scpi_query_format_com5003
  PRIVATE
    regression_data_adj_scpi_format/regression_data.qrc
)

target_sources(test_adj_regression_calc_com5003
  PRIVATE
    regression_data_adj_calc/regression_data.qrc
)

target_sources(test_adj_deny_import_com5003
  PRIVATE
    regression_data_adj_import_export/regression_data.qrc
)

target_sources(test_adj_import_invalid_eeprom_com5003
  PRIVATE
    regression_data_adj_import_export/regression_data.qrc
)
