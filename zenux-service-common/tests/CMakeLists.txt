function(SETUP_SERVICE_COMMON_TESTS)
    foreach(_testname ${ARGN})
        SETUP_TESTS(${_testname})
        target_link_libraries(${_testname}
            PUBLIC
            zenuxservicecommon-testlib
            service-interfaces-testlib
            mt310s2d-lib-testlib
        )
    endforeach()
endfunction()

SETUP_SERVICE_COMMON_TESTS(
    test_adj_data_cache
    test_adj_data_decoder
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
    test_adj_data_serialization
    )

target_sources(test_hotpluggablecontrollercontainer
  PRIVATE
  testhotplugi2cctrlcommoninfo.cpp testhotplugi2cctrlcommoninfo.h
    testhotplugctrlfactoryi2cctrl.cpp testhotplugctrlfactoryi2cctrl.h
)

target_sources(test_adj_data_cache
  PRIVATE
  AdjustedData/regression_data.qrc
)

target_sources(test_adj_data_decoder
  PRIVATE
  AdjustedData/regression_data.qrc
)

target_sources(test_adj_data_serialization
  PRIVATE
  SerializationData/regression_data.qrc
)
