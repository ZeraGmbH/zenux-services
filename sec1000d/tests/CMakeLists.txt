# Dumps dummy target to see them in IDE
file(GLOB_RECURSE TESTFILES
    test-data/*
)
# Unique target name!
add_custom_target(sec1000-test-data ALL
    SOURCES ${TESTFILES}
    )

function(SETUP_SEC1000_TESTS)
    foreach(_testname ${ARGN})
        SETUP_TESTS(${_testname})
        target_compile_definitions(${_testname}
            PRIVATE
            TEST_SRC_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
        )
        target_link_libraries(${_testname}
            PUBLIC
            zenuxservicecommon-demolib
            zenuxservicecommon-testlib
            Zera::xml-compare-testlib
            sec1000d-demolib
        )
    endforeach()
endfunction()

SETUP_SEC1000_TESTS(
    test_config_load_sec1000
    test_sec_resource
    test_regression_sec1000
    )
