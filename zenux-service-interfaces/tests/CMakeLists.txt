function(SETUP_SERVICE_INTERFACES_TESTS)
    foreach(_testname ${ARGN})
        SETUP_TESTS(${_testname})
        target_link_libraries(${_testname}
            PUBLIC
            service-interfaces
            service-interfaces-testlib
        )
    endforeach()
endfunction()

SETUP_SERVICE_INTERFACES_TESTS(
    test_tinyserverscpicmdinterface
)
