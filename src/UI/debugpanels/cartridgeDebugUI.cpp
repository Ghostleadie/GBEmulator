//
// Created by Jack_ on 04/09/2025.
//

#include "cartridgeDebugUI.h"
#include "../../components/cartridgeLoader.h"
#include "imgui.h"

void cartridgeDebugUI::updateUI(const cartridgeContext& ctx, bool* open)
{
	if (!ImGui::Begin("Rom Debug", open))
	{
		ImGui::End();
		return;
	}
	ImGui::Text("File: %s", ctx.filename);
	ImGui::Text("Size: %d bytes", ctx.romSize);
	if (ctx.header)
	{
		ImGui::Text("Title: %s", ctx.header->title);
		ImGui::Text("Type: %d", ctx.header->type);
		ImGui::Text("ROM Size: %d KB", 32 << ctx.header->romSize);
		ImGui::Text("RAM Size: %d", ctx.header->ramSize);
		ImGui::Text("Licensee Code: %d", ctx.header->licCode);
		ImGui::Text("Version: %d", ctx.header->version);
		ImGui::Text("Checksum: %d", ctx.header->checksum);
	}
	ImGui::End();
}