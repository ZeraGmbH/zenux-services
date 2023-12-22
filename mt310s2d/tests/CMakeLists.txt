SETUP_TESTS(
    test_accumulatorinterface
    test_mt310s2_config_load
    test_regression_sense_range_mt310s2
    test_regression_sense_interface_mt310s2
    )

target_sources(test_regression_sense_interface_mt310s2
  PRIVATE
    regression_data.qrc
)
