SETUP_TESTS(
    test_justdata
    test_authorizationnotifier
    test_notificationsubscriber
    test_serverunregisternotifier
    test_hotpluggablecontrollercontainer
    test_mockservice_com5003d_full
    test_mockservice_mt310s2d_full
    test_mockservice_sec1000d
    test_mockservice_zdsp1d
    test_sense_settings
    test_fpga_settings_regression
    test_criticalstatusmock
    test_readadjustmentdata
    )

target_sources(test_hotpluggablecontrollercontainer
  PRIVATE
  testhotplugi2cctrlcommoninfo.cpp testhotplugi2cctrlcommoninfo.h
    testhotplugctrlfactoryi2cctrl.cpp testhotplugctrlfactoryi2cctrl.h
)

target_sources(test_readadjustmentdata
  PRIVATE
  AdjustedData/regression_data.qrc
)
