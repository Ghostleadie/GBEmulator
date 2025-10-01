enable_testing()



add_executable(tests
        testMain.cpp
        testMain.h
        testGameboyCPU.cpp
        testMemory.h
        JsonHelper.h
        testLogging.h
        ../src/components/cpu/cpu.cpp
        ../src/components/cpu/cpu.h
        ../src/components/cpu/opcodes.cpp
        ../src/components/cpu/opcodes.h
        ../src/components/cpu/opcodeFunctions.cpp
        ../src/components/cpu/opcodeFunctions.h
        ../src/components/bus.cpp
        ../src/components/bus.h
        ../src/components/cartridgeLoader.h
        ../src/components/cartridgeLoader.cpp
        ../src/components/timer.cpp
        ../src/components/timer.h
        ../src/components/emulatorClock.cpp
        ../src/components/emulatorClock.h
)

target_compile_options(tests PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/FI$<TARGET_PROPERTY:tests,SOURCE_DIR>/testLogging.h>
)

target_link_libraries(tests
        PRIVATE
        spdlog::spdlog
        nlohmann_json
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

add_custom_command(TARGET tests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/cpu_tests
        ${CMAKE_CURRENT_BINARY_DIR}/cpu_tests
        COMMENT "Copying CPU test files to build directory"
)