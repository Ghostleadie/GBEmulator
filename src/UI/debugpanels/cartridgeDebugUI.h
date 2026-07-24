//
// Created by Jack_ on 04/09/2025.
//

#ifndef GAMEBOYEMULATOR_CARTRIDGEDEBUGUI_H
#define GAMEBOYEMULATOR_CARTRIDGEDEBUGUI_H


struct cartridgeContext;

class cartridgeDebugUI
{
public:
	/**
	 * Renders the cartridge/ROM debug panel showing parsed header fields.
	 *
	 * Shows the filename and ROM size and, when a header is present, the title,
	 * cartridge type, ROM/RAM sizes, licensee code, version, and checksum.
	 * @param ctx Cartridge context providing the filename, ROM size, and header.
	 * @param open Caller's visibility flag; the window's close button clears it.
	 */
	void updateUI(const cartridgeContext& ctx, bool* open);
};


#endif //GAMEBOYEMULATOR_CARTRIDGEDEBUGUI_H