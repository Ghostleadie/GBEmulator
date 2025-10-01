//
// Created by Jack_ on 30/09/2025.
//

#include <iostream>
#include <sstream>
#include <string>
#include <gtest/gtest.h>
#include <filesystem>
#include "testLogging.h"
#include "JsonHelper.h"
#include "testMemory.h"  // Your memory implementation
#include "../src/components/cpu/cpu.h"  // Your CPU implementation

using namespace std;

void error_callback() {}

class GameboyCPUParameterizedTest : public ::testing::TestWithParam<std::pair<std::string, CPUTestCase>> {};

TEST_P(GameboyCPUParameterizedTest, ExecuteInstruction) {
    auto [filename, testCase] = GetParam();

    std::shared_ptr<testMemory> MEM = std::make_shared<testMemory>();
	std::shared_ptr<emulatorClock> CLOCK = std::make_shared<emulatorClock>();
    cpu gb(MEM,CLOCK);

    // Set initial CPU state
    gb.getRegisters()->a = testCase.initial.a;
    gb.getRegisters()->b = testCase.initial.b;
    gb.getRegisters()->c = testCase.initial.c;
    gb.getRegisters()->d = testCase.initial.d;
    gb.getRegisters()->e = testCase.initial.e;
    gb.getRegisters()->f = testCase.initial.f;
    gb.getRegisters()->h = testCase.initial.h;
    gb.getRegisters()->l = testCase.initial.l;
    gb.getRegisters()->pc = testCase.initial.pc;
    gb.getRegisters()->sp = testCase.initial.sp;

    // Set initial memory state
	for (const auto& ramEntry : testCase.initial.ram) {
		uint16_t address = ramEntry.first;
		uint8_t value = ramEntry.second;
		gb.getBus().lock()->write(address, value);
	}

	gb.execSingleInstruction();

    // Verify final CPU state
    ASSERT_EQ(gb.getRegisters()->a, testCase.final.a) << "Register A mismatch in test: " << testCase.name;
    ASSERT_EQ(gb.getRegisters()->b, testCase.final.b) << "Register B mismatch in test: " << testCase.name;
    ASSERT_EQ(gb.getRegisters()->c, testCase.final.c) << "Register C mismatch in test: " << testCase.name;
    ASSERT_EQ(gb.getRegisters()->d, testCase.final.d) << "Register D mismatch in test: " << testCase.name;
    ASSERT_EQ(gb.getRegisters()->e, testCase.final.e) << "Register E mismatch in test: " << testCase.name;
    ASSERT_EQ(gb.getRegisters()->f, testCase.final.f) << "Register F mismatch in test: " << testCase.name;
    ASSERT_EQ(gb.getRegisters()->h, testCase.final.h) << "Register H mismatch in test: " << testCase.name;
    ASSERT_EQ(gb.getRegisters()->l, testCase.final.l) << "Register L mismatch in test: " << testCase.name;
    ASSERT_EQ(gb.getRegisters()->pc, testCase.final.pc) << "PC mismatch in test: " << testCase.name;
    ASSERT_EQ(gb.getRegisters()->sp, testCase.final.sp) << "SP mismatch in test: " << testCase.name;

    // Verify final memory state
	for (const auto& ramEntry : testCase.final.ram) {
		uint16_t address = ramEntry.first;
		uint8_t expectedValue = ramEntry.second;
		ASSERT_EQ(MEM->read(address), expectedValue)
			<< "Memory mismatch at address 0x" << std::hex << address
			<< " in test: " << testCase.name;
	}
}

// Generate test cases from JSON files
std::vector<std::pair<std::string, CPUTestCase>> generateTestCases() {
    std::vector<std::pair<std::string, CPUTestCase>> testCases;
	std::filesystem::path path = std::filesystem::current_path();
	path = path.parent_path(); // Move up directory

    std::string testDir = "tests/cpu_tests";
	path /= testDir;

    if (std::filesystem::exists(path)) {
        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            if (entry.path().extension() == ".json") {
                std::string filename = entry.path().stem().string();
            	std::cerr << "generating tests for " << filename << std::endl;
                auto tests = GameboyCPUJsonTest::loadTestsFromFile(entry.path().string());


            	for (size_t i = 0; i < tests.size(); ++i) {
            		const auto& test = tests[i];

            		// Create a valid test name by replacing invalid characters
            		std::string fullTestName = filename + "_" + std::to_string(i) + "_" + test.name;
            		std::replace_if(fullTestName.begin(), fullTestName.end(),
						[](char c) { return !std::isalnum(c) && c != '_'; }, '_');

            		testCases.emplace_back(fullTestName, test);
            	}
            }
        }
    }

    return testCases;
}

INSTANTIATE_TEST_SUITE_P(
	CPUTests,
	GameboyCPUParameterizedTest,
	::testing::ValuesIn(generateTestCases()),
	[](const ::testing::TestParamInfo<std::pair<std::string, CPUTestCase>>& info) {
		return info.param.first;
	}
);