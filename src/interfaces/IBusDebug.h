//
// Created by Jack_ on 04/09/2025.
//

#ifndef GAMEBOYEMULATOR_IBUSDEBUG_H
#define GAMEBOYEMULATOR_IBUSDEBUG_H
#include <cstdint>


class IBusDebug
{
public:
	virtual ~IBusDebug() = default;

	// Direct memory peek — doesn’t trigger DMA, VRAM block, etc.
	virtual uint8_t peek(uint16_t addr) const = 0;
};


#endif //GAMEBOYEMULATOR_IBUSDEBUG_H