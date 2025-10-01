//
// Created by Jack_ on 01/10/2025.
//

#ifndef GAMEBOYEMULATOR_JSONHELPER_H
#define GAMEBOYEMULATOR_JSONHELPER_H


#include <nlohmann/json.hpp>
#include <fstream>
#include <vector>
#include <string>

using json = nlohmann::json;

struct CPUTestCase {
	std::string name;

	// Initial state
	struct {
		uint8_t a, b, c, d, e, f, h, l;
		uint16_t pc, sp;
		std::unordered_map<uint16_t, uint16_t> ram; // [address, value] pairs
	} initial;

	// Final state
	struct {
		uint8_t a, b, c, d, e, f, h, l;
		uint16_t pc, sp;
		std::unordered_map<uint16_t, uint16_t> ram; // [address, value] pairs
	} final;

	std::vector<std::vector<std::string>> cycles; // For cycle-accurate testing
};

class GameboyCPUJsonTest {
public:
    static std::vector<CPUTestCase> loadTestsFromFile(const std::string& filepath) {
        std::vector<CPUTestCase> testCases;

        std::ifstream file(filepath);
        if (!file.is_open()) {
            return testCases; // Return empty vector if file not found
        }

        json testData;
        file >> testData;

        for (const auto& test : testData) {
            CPUTestCase testCase;
            testCase.name = test.value("name", "unknown");

            // Load initial state
            const auto& initial = test["initial"];
            testCase.initial.a = initial["a"];
            testCase.initial.b = initial["b"];
            testCase.initial.c = initial["c"];
            testCase.initial.d = initial["d"];
            testCase.initial.e = initial["e"];
            testCase.initial.f = initial["f"];
            testCase.initial.h = initial["h"];
            testCase.initial.l = initial["l"];
            testCase.initial.pc = initial["pc"];
            testCase.initial.sp = initial["sp"];

            for (const auto& ram : initial["ram"]) {
            	testCase.initial.ram[ram[0]] = ram[1];
            }

            // Load final state
            const auto& final = test["final"];
            testCase.final.a = final["a"];
            testCase.final.b = final["b"];
            testCase.final.c = final["c"];
            testCase.final.d = final["d"];
            testCase.final.e = final["e"];
            testCase.final.f = final["f"];
            testCase.final.h = final["h"];
            testCase.final.l = final["l"];
            testCase.final.pc = final["pc"];
            testCase.final.sp = final["sp"];

            for (const auto& ram : final["ram"]) {
            	testCase.final.ram[ram[0]] = ram[1];
            }

            // Load cycles
        	for (const auto& cycle : test["cycles"]) {
        		std::vector<std::string> cycleData;
        		for (const auto& item : cycle) {
        			if (item.is_string()) {
        				cycleData.push_back(item);
        			} else if (item.is_number()) {
        				cycleData.push_back(std::to_string(static_cast<int>(item)));
        			}
        		}
        		testCase.cycles.push_back(cycleData);
        	}


            testCases.push_back(testCase);
        }

        return testCases;
    }
};

#endif //GAMEBOYEMULATOR_JSONHELPER_H