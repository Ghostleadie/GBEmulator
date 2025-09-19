enable_testing()

add_executable(tests tests/testMain.cpp tests/testMain.h
        tests/testMain.cpp)

target_link_libraries(tests
        PRIVATE
        GTest::gtest
        GTest::gtest_main)

add_test(NAME testMain COMMAND tests)