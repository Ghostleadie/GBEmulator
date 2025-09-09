//
// Created by Jack_ on 07/09/2025.
//

#include "cpuDebugUI.h"

#include "imgui.h"
#include "../../components/cpu/cpu.h"

void cpuDebugUI::updateUI(const cpu& cpu)
{
	ImGui::Begin("CPU Debug");
	ImGui::Text("PC: %04X", cpu.getPC());
	ImGui::Text("SP: %04X", cpu.getRegistersDebug().sp);
	ImGui::Text("AF: %04X", cpu.getRegistersDebug().af);

	// Stepping toggle
	static bool steppingEnabled = false;
	ImGui::Checkbox("Enable Stepping", &steppingEnabled);
	//cpu.ste
	// Step button
	ImGui::BeginDisabled(!steppingEnabled);
	if (ImGui::Button("Step to Next Instruction")) {
		// You need to implement this: e.g., cpu.stepInstruction();
	}
	ImGui::EndDisabled();
}