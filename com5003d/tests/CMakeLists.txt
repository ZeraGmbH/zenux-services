SETUP_TESTS(
    test_com5003_config_load
    test_regression_sense_range_com5003
    test_regression_sense_interface_com5003
    )

target_sources(test_regression_sense_interface_com5003
  PRIVATE
    xmlfiles.qrc
)
