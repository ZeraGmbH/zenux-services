add_test(NAME    source-control-common-unittest
         COMMAND source-control-common-unittest)

file(GLOB UNITTESTS
    *.cpp
)

add_executable(source-control-common-unittest
    ${UNITTESTS}
)
# constexpr / EXPECT_EQ -> linker problems for c++ < 17
set_property(TARGET source-control-common-unittest PROPERTY CXX_STANDARD 17)

target_include_directories(source-control-common-unittest
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    )

target_link_libraries(source-control-common-unittest
    PUBLIC
    gtest
    gtest_main
    source-json-io-lib
)
