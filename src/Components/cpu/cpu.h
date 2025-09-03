//
// Created by Jack_ on 31/08/2025.
//

#ifndef GAMEBOYEMULATOR_CPU_H
#define GAMEBOYEMULATOR_CPU_H
#include "../base/component.h"

class bus;

struct registers
{
    struct {
        union {
            struct {
                unsigned char f;
                unsigned char a;
            };
            unsigned short af;
        };
    };

    struct {
        union {
            struct {
                unsigned char c;
                unsigned char b;
            };
            unsigned short bc;
        };
    };

    struct {
        union {
            struct {
                unsigned char e;
                unsigned char d;
            };
            unsigned short de;
        };
    };

    struct {
        union {
            struct {
                unsigned char l;
                unsigned char h;
            };
            unsigned short hl;
        };
    };

    unsigned short sp;
    unsigned short pc;
};

struct flags
{
	bool zero;
	bool subtraction;
	bool halfCarry;
	bool carry;
};

class cpu
{
public:
	cpu(std::shared_ptr<bus> bus) : m_bus(bus) {};
	void init();
	void emulateCycle();
	uint8_t fetch();
	void execute(uint8_t opcode);

	void onInstructionExecuted(const std::string& instruction);


private:
	std::shared_ptr<bus> m_bus;

	registers registers = {};
	uint16_t fetchedData;
	uint16_t memoryDestination;
	uint8_t currentOpcode;
	uint8_t interruptEnableRegister;
public:

private:
};

#endif //GAMEBOYEMULATOR_CPU_H