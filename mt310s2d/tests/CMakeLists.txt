add_library(senserangetesttemplate STATIC
    senserangetesttemplate.cpp
    senserangetesttemplate.h
)

target_include_directories(senserangetesttemplate
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(senserangetesttemplate
    PUBLIC
    Qt5::Test
    mt310s2d-lib
    )

SETUP_TESTS(
    test_senserange
    test_accumulatorinterface
    )
