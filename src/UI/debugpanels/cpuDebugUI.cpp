//
// Created by Jack_ on 07/09/2025.
//

#include "cpuDebugUI.h"

#include "imgui.h"
#include "../../components/cpu/cpu.h"

void cpuDebugUI::updateUI(cpu& cpu)
{
	ImGui::Begin("CPU Debug");
	ImGui::Text("PC: %04X  SP: %04X", cpu.getPC(), cpu.getRegistersDebug().sp);
	ImGui::Text("A: %04X F: %04X AF: %04X", cpu.getRegistersDebug().a, cpu.getRegistersDebug().f, cpu.getRegistersDebug().af);
	ImGui::Text("B: %04X C: %04X BC: %04X", cpu.getRegistersDebug().b, cpu.getRegistersDebug().c, cpu.getRegistersDebug().bc);
	ImGui::Text("D: %04X E: %04X DE: %04X", cpu.getRegistersDebug().d, cpu.getRegistersDebug().e, cpu.getRegistersDebug().de);
	ImGui::Text("H: %04X L: %04X HL: %04X", cpu.getRegistersDebug().h, cpu.getRegistersDebug().l, cpu.getRegistersDebug().hl);
	ImGui::NewLine();
	ImGui::Text("Flags: Z=%d N=%d H=%d C=%d", cpu.isZeroFlagSet(), cpu.isSubtractFlagSet(), cpu.isHalfCarryFlagSet(), cpu.isCarryFlagSet());

	// Stepping toggle
	static bool steppingEnabled = false;
	ImGui::Checkbox("Enable Stepping", &steppingEnabled);

	if (cpu.getSteppingMode() != steppingEnabled) {
		cpu.setSteppingMode(steppingEnabled);
	}

	// Step button
	ImGui::BeginDisabled(!steppingEnabled);
	if (ImGui::Button("Step to Next Instruction")) {
		if (cpu.getStepComplete() == true)
		{
			cpu.setStepComplete(false);
		}
	}
	ImGui::EndDisabled();
	ImGui::End();
}