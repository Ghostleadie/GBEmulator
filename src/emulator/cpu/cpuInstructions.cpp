#include "cpu.h"
#include "../emulator.h"
#include "../bus.h"

void cpu::instructionNOP()
{
	return;
}

void cpu::instructionLD(std::weak_ptr<bus> bus)
{
    if (auto m_bus = bus.lock())
    {
        if (ctx->destinationIsMemory) {
            //LD (BC), A for instance...

            if (utility::isReg16Bit(ctx->currentInstruction.reg2)) {
                //if 16 bit register...
                emulation::cycles(1);
                m_bus->write(ctx->memoryDestination, ctx->fetchedData);
            }
            else {
                m_bus->write(ctx->memoryDestination, ctx->fetchedData);
            }

            emulation::cycles(1);

            return;
        }
    }
    else
    {
        GBE_ERROR("Bus does not exisit");
    }

    if (ctx->currentInstruction.mode == AM_HL_SPR) {
        uint8_t hflag = (readRegistry(ctx->currentInstruction.reg2) & 0xF) + (ctx->fetchedData & 0xF) >= 0x10;

        uint8_t cflag = (readRegistry(ctx->currentInstruction.reg2) & 0xFF) + (ctx->fetchedData & 0xFF) >= 0x100;
        
        setFlags(ctx, 0, 0, hflag, cflag);
        setRegistry(ctx->currentInstruction.reg1, (uint16_t)(readRegistry(ctx->currentInstruction.reg2) + (char)ctx->fetchedData));

        return;
    }

    setRegistry(ctx->currentInstruction.reg1, ctx->fetchedData);
}

void cpu::instructionINC(std::weak_ptr<bus> bus)
{
	if (auto m_bus = bus.lock())
	{
		//8-bits
		uint16_t value = readRegistry(ctx->currentInstruction.reg1) + 1;
		if (utility::isReg16Bit(ctx->currentInstruction.reg1))
		{
			emulation::cycles(1);
		}
		//16-bits (HL registry)
		if (ctx->currentInstruction.reg1 == RT_HL && ctx->currentInstruction.mode == AM_MR)
		{
			//increasing the HL value
			value = m_bus->read(readRegistry(RT_HL)) + 1;
			value &= 0xFF;
			m_bus->write(readRegistry(RT_HL), value);
		}
		else
		{
			setRegistry(ctx->currentInstruction.reg1, value);

		}

		if ((ctx->currentOpcode & 0x03) == 0x03)
		{
			return;
		}
		setFlags(ctx, value == 0, 0, (value & 0x0F) == 0, -1);
	}
}

void cpu::instructionDEC(std::weak_ptr<bus> bus)
{
	if (auto m_bus = bus.lock())
	{
		//8-bits
		uint16_t value = readRegistry(ctx->currentInstruction.reg1) - 1;
		if (utility::isReg16Bit(ctx->currentInstruction.reg1))
		{
			emulation::cycles(1);
		}
		//16-bits (HL registry)
		if (ctx->currentInstruction.reg1 == RT_HL && ctx->currentInstruction.mode == AM_MR)
		{
			//increasing the HL value
			value = m_bus->read(readRegistry(RT_HL)) - 1;
			value &= 0xFF;
			m_bus->write(readRegistry(RT_HL), value);
		}
		else
		{
			setRegistry(ctx->currentInstruction.reg1, value);

		}

		if ((ctx->currentOpcode & 0x0B) == 0x0B)
		{
			return;
		}

		setFlags(ctx, value == 0, 1, (value & 0x0F) == 0x0F, -1);
	}
}

void cpu::instructionRLCA()
{
    uint8_t u = ctx->registers.a;
    bool c = (u >> 7) & 1;
    u = (u << 1) | c;
    ctx->registers.a = u;

    setFlags(ctx, 0, 0, 0, c);
}

void cpu::instructionADD()
{
	//32 bits incase of overflow
	uint32_t value = readRegistry(ctx->currentInstruction.reg1) + ctx->fetchedData;

	if (utility::isReg16Bit(ctx->currentInstruction.reg1))
	{
		emulation::cycles(1);
	}

	if (ctx->currentInstruction.reg1 == RT_SP)
	{
		value = readRegistry(ctx->currentInstruction.reg1) + static_cast<char>(ctx->fetchedData);
	}

    int z = (value & 0xFF) == 0;
    int h = (readRegistry(ctx->currentInstruction.reg1) & 0xF) + (ctx->fetchedData & 0xF) >= 0x10;
    int c = (int)(readRegistry(ctx->currentInstruction.reg1) & 0xFF) + (int)(ctx->fetchedData & 0xFF) >= 0x100;

    if (utility::isReg16Bit(ctx->currentInstruction.reg1)) {
        z = -1;
        h = (readRegistry(ctx->currentInstruction.reg1) & 0xFFF) + (ctx->fetchedData & 0xFFF) >= 0x1000;
        uint32_t n = ((uint32_t)readRegistry(ctx->currentInstruction.reg1)) + ((uint32_t)ctx->fetchedData);
        c = n >= 0x10000;
    }

    if (ctx->currentInstruction.reg1 == RT_SP) {
        z = 0;
        h = (readRegistry(ctx->currentInstruction.reg1) & 0xF) + (ctx->fetchedData & 0xF) >= 0x10;
        c = (int)(readRegistry(ctx->currentInstruction.reg1) & 0xFF) + (int)(ctx->fetchedData & 0xFF) > 0x100;
    }

    setRegistry(ctx->currentInstruction.reg1, (uint16_t)(value & 0xFFFF));
    setFlags(ctx, z, 0, h, c);
}

void cpu::instructionRRCA()
{
    uint8_t b = ctx->registers.a & 1;
    ctx->registers.a >>= 1;
    ctx->registers.a |= (b << 7);

    setFlags(ctx, 0, 0, 0, b);
}

void cpu::instructionSTOP()
{
    exit(-7);
}

void cpu::instructionRLA()
{
    uint8_t u = ctx->registers.a;
    uint8_t cf = utility::checkBit(ctx->registers.f, 4);
    uint8_t c = (u >> 7) & 1;

    ctx->registers.a = (u << 1) | cf;
    setFlags(ctx, 0, 0, 0, c);
}

void cpu::instructionJR()
{
    char rel = (char)(ctx->fetchedData & 0xFF);
    uint16_t addr = ctx->registers.programCounter + rel;
    goToAddress(ctx, addr, false);
}

void cpu::instructionRRA()
{
    uint8_t carry = utility::checkBit(ctx->registers.f, 4);
    uint8_t new_c = ctx->registers.a & 1;

    ctx->registers.a >>= 1;
    ctx->registers.a |= (carry << 7);

    setFlags(ctx, 0, 0, 0, new_c);
}

void cpu::instructionDAA()
{
    uint8_t u = 0;
    int fc = 0;

    if (utility::checkBit(ctx->registers.f, 5) || (!utility::checkBit(ctx->registers.f, 6) && (ctx->registers.a & 0xF) > 9)) {
        u = 6;
    }

    if (utility::checkBit(ctx->registers.f, 4) || (!utility::checkBit(ctx->registers.f, 6) && ctx->registers.a > 0x99)) {
        u |= 0x60;
        fc = 1;
    }

    ctx->registers.a += utility::checkBit(ctx->registers.f, 6) ? -u : u;

    setFlags(ctx, ctx->registers.a == 0, -1, 0, fc);
}

void cpu::instructionCPL()
{
    ctx->registers.a = ~ctx->registers.a;
    setFlags(ctx, -1, 1, 1, -1);
}

void cpu::instructionSCF()
{
    setFlags(ctx, -1, 0, 0, 1);
}

void cpu::instructionCCF()
{
    setFlags(ctx, -1, 0, 0, utility::checkBit(ctx->registers.f, 4) ^ 1);
}

void cpu::instructionHALT()
{
    ctx->halted = true;
}

void cpu::instructionADC()
{
    uint16_t u = ctx->fetchedData;
    uint16_t a = ctx->registers.a;
    uint16_t c = utility::checkBit(ctx->registers.f, 4);

    ctx->registers.a = (a + u + c) & 0xFF;

    setFlags(ctx, ctx->registers.a == 0, 0, (a & 0xF) + (u & 0xF) + c > 0xF, a + u + c > 0xFF);
}

void cpu::instructionSUB()
{
    uint16_t val = readRegistry(ctx->currentInstruction.reg1) - ctx->fetchedData;

    int z = val == 0;
    int h = ((int)readRegistry(ctx->currentInstruction.reg1) & 0xF) - ((int)ctx->fetchedData & 0xF) < 0;
    int c = ((int)readRegistry(ctx->currentInstruction.reg1)) - ((int)ctx->fetchedData) < 0;

    setRegistry(ctx->currentInstruction.reg1, val);
    setFlags(ctx, z, 1, h, c);
}

void cpu::instructionSBC()
{
    uint8_t val = ctx->fetchedData + utility::checkBit(ctx->registers.f, 4);

    int z = readRegistry(ctx->currentInstruction.reg1) - val == 0;

    int h = ((int)readRegistry(ctx->currentInstruction.reg1) & 0xF) - ((int)ctx->fetchedData & 0xF) - ((int)utility::checkBit(ctx->registers.f, 4)) < 0;
    int c = ((int)readRegistry(ctx->currentInstruction.reg1)) - ((int)ctx->fetchedData) - ((int)utility::checkBit(ctx->registers.f, 4)) < 0;

    setRegistry(ctx->currentInstruction.reg1, (uint16_t)(readRegistry(ctx->currentInstruction.reg1) - val));
    setFlags(ctx, z, 1, h, c);
}

void cpu::instructionAND()
{
    ctx->registers.a &= ctx->fetchedData & 0xFF;
    setFlags(ctx, ctx->registers.a == 0, 0, 1, 0);
}

void cpu::instructionXOR()
{
    ctx->registers.a ^= ctx->fetchedData & 0xFF;
    setFlags(ctx, ctx->registers.a == 0, 0, 0, 0);
}

void cpu::instructionOR()
{
    ctx->registers.a |= ctx->fetchedData & 0xFF;
    setFlags(ctx, ctx->registers.a == 0, 0, 0, 0);
}

void cpu::instructionCP()
{
    int n = (int)ctx->registers.a - (int)ctx->fetchedData;

    setFlags(ctx, n == 0, 1, ((int)ctx->registers.a & 0x0F) - ((int)ctx->fetchedData & 0x0F) < 0, n < 0);
}

void cpu::instructionPOP()
{
    uint16_t lo = popStack();
    emulation::cycles(1);
    uint16_t hi = popStack();
    emulation::cycles(1);

    uint16_t n = (hi << 8) | lo;

    setRegistry(ctx->currentInstruction.reg1, n);

    if (ctx->currentInstruction.reg1 == RT_AF)
    {
        setRegistry(ctx->currentInstruction.reg1, (uint16_t)(n & 0xFFF0));
    }
}

void cpu::instructionJP()
{
    goToAddress(ctx, ctx->fetchedData, false);
}

void cpu::instructionPUSH()
{
    uint16_t hi = (readRegistry(ctx->currentInstruction.reg1) >> 8) & 0xFF;
    emulation::cycles(1);
    pushStack(hi);

    uint16_t lo = readRegistry(ctx->currentInstruction.reg1) & 0xFF;
    emulation::cycles(1);
    pushStack(lo);

    emulation::cycles(1);
}

void cpu::instructionRET()
{
    if (ctx->currentInstruction.cond != CT_NONE) {
        emulation::cycles(1);
    }

    if (checkCondition(ctx)) {
        uint16_t lo = popStack();
        emulation::cycles(1);
        uint16_t hi = popStack();
        emulation::cycles(1);

        uint16_t n = (hi << 8) | lo;
        ctx->registers.programCounter = n;

        emulation::cycles(1);
    }
}

void cpu::instructionCB()
{
    uint8_t op = ctx->fetchedData;
    registryType reg = instructions::registryLookup(op & 0b111);
    uint8_t bit = (op >> 3) & 0b111;
    uint8_t bit_op = (op >> 6) & 0b11;
    uint8_t registerValue = cbReadReg(reg);

    emulation::cycles(1);

    if (reg == RT_HL) {
        emulation::cycles(2);
    }

    switch (bit_op) {
    case 1:
        //BIT
        setFlags(ctx, !(registerValue & (1 << bit)), 0, 1, -1);
        return;

    case 2:
        //RST
        registerValue &= ~(1 << bit);
        cbsetReg(reg, registerValue);
        return;

    case 3:
        //SET
        registerValue |= (1 << bit);
        cbsetReg(reg, registerValue);
        return;
    }

    bool flagC = utility::checkBit(ctx->registers.f, 4);

    switch (bit) {
    case 0: 
    {
        //RLC
        bool setC = false;
        uint8_t result = (registerValue << 1) & 0xFF;

        if ((registerValue & (1 << 7)) != 0) {
            result |= 1;
            setC = true;
        }

        cbsetReg(reg, result);
        setFlags(ctx, result == 0, false, false, setC);
    } 
          return;

    case 1: 
    {
        //RRC
        uint8_t old = registerValue;
        registerValue >>= 1;
        registerValue |= (old << 7);

        cbsetReg(reg, registerValue);
        setFlags(ctx, !registerValue, false, false, old & 1);
    } 
         return;

    case 2: 
    {
        //RL
        uint8_t old = registerValue;
        registerValue <<= 1;
        registerValue |= flagC;

        cbsetReg(reg, registerValue);
        setFlags(ctx, !registerValue, false, false, !!(old & 0x80));
    } 
        return;

    case 3: 
    {
        //RR
        uint8_t old = registerValue;
        registerValue >>= 1;

        registerValue |= (flagC << 7);

        cbsetReg(reg, registerValue);
        setFlags(ctx, !registerValue, false, false, old & 1);
    }
        return;

    case 4:
    {
        //SLA
        uint8_t old = registerValue;
        registerValue <<= 1;

        cbsetReg(reg, registerValue);
        setFlags(ctx, !registerValue, false, false, !!(old & 0x80));
    }
        return;

    case 5: 
    {
        //SRA
        uint8_t u = (int8_t)registerValue >> 1;
        cbsetReg(reg, u);
        setFlags(ctx, !u, 0, 0, registerValue & 1);
    }
        return;

    case 6: 
    {
        //SWAP
        registerValue = ((registerValue & 0xF0) >> 4) | ((registerValue & 0xF) << 4);
        cbsetReg(reg, registerValue);
        setFlags(ctx, registerValue == 0, false, false, false);
    }
        return;

    case 7:
    {
        //SRL
        uint8_t u = registerValue >> 1;
        cbsetReg(reg, u);
        setFlags(ctx, !u, 0, 0, registerValue & 1);
    }
        return;
    }
}

void cpu::instructionCALL()
{
    goToAddress(ctx, ctx->fetchedData, true);
}

void cpu::instructionRETI()
{
    ctx->masterInterruptEnabled = true;
    instructionRET();
}

void cpu::instructionLDH(std::weak_ptr<bus> bus)
{
    if (auto m_bus = bus.lock())
    {
        if (ctx->currentInstruction.reg1 == RT_A) {
            setRegistry(ctx->currentInstruction.reg1, m_bus->read(0xFF00 | ctx->fetchedData));
        }
        else {
            m_bus->write(ctx->memoryDestination, ctx->registers.a);
        }

        emulation::cycles(1);
    }
}

void cpu::instructionJPHL()
{
    exit(-7);
}

void cpu::instructionDI()
{
    ctx->masterInterruptEnabled = false;
}

void cpu::instructionEI()
{
    ctx->enableIME = true;
}

void cpu::instructionRST()
{
    goToAddress(ctx, ctx->currentInstruction.param, true);
}
