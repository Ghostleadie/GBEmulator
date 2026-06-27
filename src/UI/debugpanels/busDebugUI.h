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
 void updateUI(bus& m_bus);
private:
	std::size_t lastSerialLen = 0; // tracks growth for auto-scroll
};


#endif //GAMEBOYEMULATOR_BUSDEBUGUI_H
