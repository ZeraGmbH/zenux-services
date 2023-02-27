add_definitions(-DCONFIG_PATH="${CMAKE_CURRENT_SOURCE_DIR}/../../mt310s2d/configs")

SETUP_TESTS(
    test_justdata
    test_asynchnotificationsinterface
    test_authorizationnotifier
    test_notificationsubscriber
    test_serverunregisternotifier
    )
