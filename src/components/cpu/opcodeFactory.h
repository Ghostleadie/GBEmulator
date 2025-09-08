//
// Created by Jack_ on 08/09/2025.
//

#ifndef GAMEBOYEMULATOR_OPCODEFACTORY_H
#define GAMEBOYEMULATOR_OPCODEFACTORY_H
#include <cstdint>

#include "opcodes.h"


class opcodeFactory
{
public:
	static opcode create(uint8_t opcode);
};


#endif //GAMEBOYEMULATOR_OPCODEFACTORY_H