function(SETUP_SYSTEM_METRICE_TESTS)
    foreach(_testname ${ARGN})
        SETUP_TESTS(${_testname})
        target_sources(${_testname}
            PRIVATE
            test-data/test-data.qrc
        )
        target_link_libraries(${_testname}
            PUBLIC
            system-metrics
            system-metrics-testlib
        )
    endforeach()
endfunction()

SETUP_SYSTEM_METRICE_TESTS(
    test_procstatdecoder
    test_cpuload
    test_procmeminfodecoder
    test_totalmemorytracker
    test_cputemp
    test_cpufreq
    test_fpgainterrupts
    test_procinterruptdecoder
    test_logstrategy
    test_disk_read_write
    test_disk_io_calculator
)
