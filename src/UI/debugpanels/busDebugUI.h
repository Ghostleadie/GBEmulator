//
// Created by Jack_ on 05/09/2025.
//

#ifndef GAMEBOYEMULATOR_BUSDEBUGUI_H
#define GAMEBOYEMULATOR_BUSDEBUGUI_H
#include "../../components/bus.h"
#include <cstddef>


class busDebugUI
{
public:
 /**
  * Renders the bus debug panel showing captured serial output (read-only).
  *
  * The bus records every write to 0xFF01, so the full serial stream is shown
  * without racing the CPU for the 0xFF02 transfer (the old panel stole bytes and
  * usually showed nothing). Auto-scrolls as output arrives and offers a Clear
  * button and a bus-side serial-logging toggle.
  * @param m_bus Bus supplying the serial output and serial-logging flag.
  * @param open Caller's visibility flag; the window's close button clears it.
  */
 void updateUI(bus& m_bus, bool* open);
private:
	std::size_t lastSerialLen = 0; // tracks growth for auto-scroll
};


#endif //GAMEBOYEMULATOR_BUSDEBUGUI_H
