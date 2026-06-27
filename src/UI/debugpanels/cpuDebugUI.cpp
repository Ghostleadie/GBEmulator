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
	ImGui::Text("A: %02X  F: %02X  AF: %04X", cpu.getRegistersDebug().a, cpu.getRegistersDebug().f, cpu.getRegistersDebug().af);
	ImGui::Text("B: %02X  C: %02X  BC: %04X", cpu.getRegistersDebug().b, cpu.getRegistersDebug().c, cpu.getRegistersDebug().bc);
	ImGui::Text("D: %02X  E: %02X  DE: %04X", cpu.getRegistersDebug().d, cpu.getRegistersDebug().e, cpu.getRegistersDebug().de);
	ImGui::Text("H: %02X  L: %02X  HL: %04X", cpu.getRegistersDebug().h, cpu.getRegistersDebug().l, cpu.getRegistersDebug().hl);
	ImGui::NewLine();
	ImGui::Text("Flags: Z=%d N=%d H=%d C=%d", cpu.isZeroFlagSet(), cpu.isSubtractFlagSet(), cpu.isHalfCarryFlagSet(), cpu.isCarryFlagSet());

	// Stepping toggle
	static bool steppingEnabled = false;
	ImGui::Checkbox("Enable Stepping", &steppingEnabled);

	if (cpu.getSteppingMode() != steppingEnabled) {
		cpu.setSteppingMode(steppingEnabled);
	}

	// CPU instruction trace toggle (Gameboy Doctor -> trace.txt)
	static bool traceEnabled = false;
	ImGui::Checkbox("Enable CPU Trace (Gameboy Doctor)", &traceEnabled);

	if (cpu.getTraceLogging() != traceEnabled) {
		cpu.setTraceLogging(traceEnabled);
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

	const auto  draw_list_size = ImVec2(310, 260);

	/*if (ImGui::BeginListBox("##opcode history", draw_list_size)) {
		const auto& opcodes = cpu.getOpcodesHistory();
		static int selected_idx = -1;

		for (int n = 0; n < opcodes.size(); ++n) {
			const bool is_selected = (selected_idx == n);

			if (ImGui::Selectable(opcodes[n].c_str(), is_selected)) {
				selected_idx = n;
			}

			// Auto-scroll to the latest entry
			if (n == opcodes.size() - 1) {
				ImGui::SetScrollHereY(1.0f);
			}

			if (is_selected) {
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndListBox();*/
	//}
	ImGui::End();
}