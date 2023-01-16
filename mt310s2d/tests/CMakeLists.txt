# Inspired by
# https://github.com/KDAB/kdabtv/blob/master/Qt-Widgets-and-more/UnitTests/Tests/CMakeLists.txt

find_package(Qt5Test REQUIRED)

add_library(testhelpers STATIC
    senserangetesttemplate.cpp
    senserangetesttemplate.h
)

target_include_directories(testhelpers
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(testhelpers
    PUBLIC
    Qt5::Test
    mt310s2d-lib
    )

SETUP_TESTS(
    test_senserange
    )
