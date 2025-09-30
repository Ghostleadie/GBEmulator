enable_testing()

add_executable(tests
        testMain.cpp
        testMain.h
)

target_link_libraries(tests
        PRIVATE
        gtest
        gtest_main)

add_test(NAME testMain COMMAND tests)

add_custom_command(TARGET tests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:gtest>
        $<TARGET_FILE_DIR:tests>
)

add_custom_command(TARGET tests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:gtest_main>
        $<TARGET_FILE_DIR:tests>
)