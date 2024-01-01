SETUP_TESTS(
    test_com5003_config_load
    test_regression_sense_range_com5003
    test_regression_sense_interface_com5003
    test_regression_adj_import_export_xml_com5003
    )

target_sources(test_regression_sense_interface_com5003
  PRIVATE
    regression_data_sense_constants/regression_data.qrc
)

target_sources(test_regression_adj_import_export_xml_com5003
  PRIVATE
    regression_data_adj_xml/regression_data.qrc
)
