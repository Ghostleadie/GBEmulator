//
// Created by Jack_ on 08/09/2025.
//

#include "opcodeFunctions.h"
#include "cpu.h"
#include "../../emulator.h"
#include "../../components/bus.h"
#include "../../Utility/utility.h"
#include <unordered_map>

void AdcCommand::execute(cpu& m_cpu) {}

void Add8BitCommand::execute(cpu& m_cpu)
{
	uint8_t reg1Val = m_cpu.readRegister(m_cpu.getCurrentOpcodeData().reg1) & 0xFF;
	uint8_t fetchedVal = m_cpu.getFetchedData() & 0xFF;
	uint16_t result = reg1Val + fetchedVal;

	int z = (result & 0xFF) == 0;
	int h = (reg1Val & 0xF) + (fetchedVal & 0xF) >= 0x10;
	int c = result > 0xFF;

	m_cpu.writeRegister(m_cpu.getCurrentOpcodeData().reg1, result & 0xFF);
	m_cpu.setFlags(z, 0, h, c);
}

void Add16BitCommand::execute(cpu& m_cpu)
{
	uint16_t reg1Val = m_cpu.readRegister(m_cpu.getCurrentOpcodeData().reg1);
	uint16_t fetchedVal = m_cpu.getFetchedData();
	uint32_t result = reg1Val + fetchedVal;

	emulator::cycles(1);

	int h = (reg1Val & 0xFFF) + (fetchedVal & 0xFFF) >= 0x1000;
	int c = result >= 0x10000;

	m_cpu.writeRegister(m_cpu.getCurrentOpcodeData().reg1, result & 0xFFFF);
	m_cpu.setFlags(-1, 0, h, c);
}

void AddSPCommand::execute(cpu& m_cpu)
{
	uint16_t sp = m_cpu.readRegister(RT_SP);
	int8_t offset = static_cast<int8_t>(m_cpu.getFetchedData() & 0xFF);
	uint16_t result = sp + offset;

	int h = (sp & 0xF) + (offset & 0xF) >= 0x10;
	int c = (sp & 0xFF) + (offset & 0xFF) > 0xFF;

	m_cpu.writeRegister(RT_SP, result);
	m_cpu.setFlags(0, 0, h, c);
}

void AndCommand::execute(cpu& m_cpu) {}

void CallCommand::execute(cpu& m_cpu) {}

void CcfCommand::execute(cpu& m_cpu) {}

void CbCommand::execute(cpu& m_cpu) {}

void CplCommand::execute(cpu& m_cpu) {}

void CpCommand::execute(cpu& m_cpu) {}

void DaaCommand::execute(cpu& m_cpu) {}

void DecCommand::execute(cpu& m_cpu) {}

void DiCommand::execute(cpu& m_cpu) {}

void EiCommand::execute(cpu& m_cpu) {}

void HaltCommand::execute(cpu& m_cpu) {}

void IncCommand::execute(cpu& m_cpu) {}

void JpCommand::execute(cpu& m_cpu)
{
	if (m_cpu.checkConditionFlags())
	{
		m_cpu.getRegisters().pc = m_cpu.getFetchedData();
		emulator::cycles(1);
	}
}

void JphlCommand::execute(cpu& m_cpu) {}

void JrCommand::execute(cpu& m_cpu) {}

void Ld8BitCommand::execute(cpu& m_cpu)
{
	if (m_cpu.getDestinationIsMemory()) {
		//LD (BC), A for instance...
		
		m_cpu.getBus().lock()->write(m_cpu.getMemoryDestination(), m_cpu.getFetchedData());
		
		emulator::cycles(1);

		return;
	}
	m_cpu.writeRegister(m_cpu.getCurrentOpcodeData().reg1, m_cpu.getFetchedData());
}

void Ld16BitCommand::execute(cpu& m_cpu)
{
	if (m_cpu.getDestinationIsMemory()) {
		//LD (BC), A for instance...
		emulator::cycles(1);
		m_cpu.getBus().lock()->write(m_cpu.getMemoryDestination(), m_cpu.getFetchedData());
		emulator::cycles(1);

		return;
	}
	m_cpu.writeRegister(m_cpu.getCurrentOpcodeData().reg1, m_cpu.getFetchedData());
}

void LdSpecialCommand::execute(cpu& m_cpu)
{
	int8_t offset = static_cast<int8_t>(m_cpu.getFetchedData() & 0xFF);
	uint16_t sp = m_cpu.readRegister(RT_SP);
	uint16_t result = static_cast<uint16_t>(sp + offset);

	int h = ((sp & 0xF) + (offset & 0xF)) >= 0x10;
	int c = ((sp & 0xFF) + (offset & 0xFF)) >= 0x100;

	m_cpu.writeRegister(RT_HL, result);
	m_cpu.setFlags(0, 0, h, c);
	emulator::cycles(1);
}

void LdhCommand::execute(cpu& m_cpu) {}

void NopCommand::execute(cpu& m_cpu) {}

void OrCommand::execute(cpu& m_cpu) {}

void PopCommand::execute(cpu& m_cpu) {}

void PushCommand::execute(cpu& m_cpu) {}

void RetCommand::execute(cpu& m_cpu) {}

void RetiCommand::execute(cpu& m_cpu) {}

void RlaCommand::execute(cpu& m_cpu) {}

void RlcaCommand::execute(cpu& m_cpu) {}

void RraCommand::execute(cpu& m_cpu) {}

void RrcaCommand::execute(cpu& m_cpu) {}

void RstCommand::execute(cpu& m_cpu) {}

void SbcCommand::execute(cpu& m_cpu) {}

void ScfCommand::execute(cpu& m_cpu) {}

void StopCommand::execute(cpu& m_cpu) {}

void SubCommand::execute(cpu& m_cpu) {}

void XorCommand::execute(cpu& m_cpu) {}

std::unique_ptr<OpcodeCommand> OpcodeCommandFactory::createCommand(const uint8_t opcode) {
	 // NOP
    if (opcode == 0x00) return std::make_unique<NopCommand>();

    // LD 16-bit immediate
    if ((opcode & 0xCF) == 0x01) return std::make_unique<Ld16BitCommand>();

    // LD (reg16), A
    if (opcode == 0x02 || opcode == 0x12 || opcode == 0x22 || opcode == 0x32)
        return std::make_unique<LdSpecialCommand>();

    // INC 16-bit
    if ((opcode & 0xCF) == 0x03) return std::make_unique<IncCommand>();

    // INC 8-bit
    if ((opcode & 0xC7) == 0x04) return std::make_unique<IncCommand>();

    // DEC 8-bit
    if ((opcode & 0xC7) == 0x05) return std::make_unique<DecCommand>();

    // LD 8-bit immediate
    if ((opcode & 0xC7) == 0x06) return std::make_unique<Ld8BitCommand>();

    // Rotate A instructions
    if (opcode == 0x07) return std::make_unique<RlcaCommand>();
    if (opcode == 0x0F) return std::make_unique<RrcaCommand>();
    if (opcode == 0x17) return std::make_unique<RlaCommand>();
    if (opcode == 0x1F) return std::make_unique<RraCommand>();

    // ADD HL, reg16
    if ((opcode & 0xCF) == 0x09) return std::make_unique<Add16BitCommand>();

    // LD A, (reg16)
    if (opcode == 0x0A || opcode == 0x1A || opcode == 0x2A || opcode == 0x3A)
        return std::make_unique<LdSpecialCommand>();

    // DEC 16-bit
    if ((opcode & 0xCF) == 0x0B) return std::make_unique<DecCommand>();

    // JR instructions
    if (opcode == 0x18 || (opcode & 0xE7) == 0x20) return std::make_unique<JrCommand>();

    // LD 8-bit register to register (0x40-0x7F except 0x76)
    if ((opcode & 0xC0) == 0x40 && opcode != 0x76) return std::make_unique<Ld8BitCommand>();

    // HALT
    if (opcode == 0x76) return std::make_unique<HaltCommand>();

    // 8-bit ADD
    if ((opcode & 0xF8) == 0x80) return std::make_unique<Add8BitCommand>();

    // 8-bit ADC
    if ((opcode & 0xF8) == 0x88) return std::make_unique<AdcCommand>();

    // 8-bit SUB
    if ((opcode & 0xF8) == 0x90) return std::make_unique<SubCommand>();

    // 8-bit SBC
    if ((opcode & 0xF8) == 0x98) return std::make_unique<SbcCommand>();

    // 8-bit AND
    if ((opcode & 0xF8) == 0xA0) return std::make_unique<AndCommand>();

    // 8-bit XOR
    if ((opcode & 0xF8) == 0xA8) return std::make_unique<XorCommand>();

    // 8-bit OR
    if ((opcode & 0xF8) == 0xB0) return std::make_unique<OrCommand>();

    // 8-bit CP
    if ((opcode & 0xF8) == 0xB8) return std::make_unique<CpCommand>();

    // Conditional RET
    if ((opcode & 0xE7) == 0xC0) return std::make_unique<RetCommand>();

    // POP
    if ((opcode & 0xCF) == 0xC1) return std::make_unique<PopCommand>();

    // Conditional JP
    if ((opcode & 0xE7) == 0xC2) return std::make_unique<JpCommand>();

    // JP absolute
    if (opcode == 0xC3) return std::make_unique<JpCommand>();

    // Conditional CALL
    if ((opcode & 0xE7) == 0xC4) return std::make_unique<CallCommand>();

    // PUSH
    if ((opcode & 0xCF) == 0xC5) return std::make_unique<PushCommand>();

    // ADD/ADC/SUB/SBC/AND/XOR/OR/CP immediate
    if ((opcode & 0xC7) == 0xC6) {
        switch (opcode & 0x38) {
            case 0x00: return std::make_unique<Add8BitCommand>();
            case 0x08: return std::make_unique<AdcCommand>();
            case 0x10: return std::make_unique<SubCommand>();
            case 0x18: return std::make_unique<SbcCommand>();
            case 0x20: return std::make_unique<AndCommand>();
            case 0x28: return std::make_unique<XorCommand>();
            case 0x30: return std::make_unique<OrCommand>();
            case 0x38: return std::make_unique<CpCommand>();
        }
    }

    // RST
    if ((opcode & 0xC7) == 0xC7) return std::make_unique<RstCommand>();

    // RET
    if (opcode == 0xC9) return std::make_unique<RetCommand>();

    // CB prefix
    if (opcode == 0xCB) return std::make_unique<CbCommand>();

    // CALL
    if (opcode == 0xCD) return std::make_unique<CallCommand>();

    // Misc single-byte instructions
    switch (opcode) {
        case 0x08: return std::make_unique<LdSpecialCommand>(); // LD (nn), SP
        case 0x10: return std::make_unique<StopCommand>();
        case 0x27: return std::make_unique<DaaCommand>();
        case 0x2F: return std::make_unique<CplCommand>();
        case 0x37: return std::make_unique<ScfCommand>();
        case 0x3F: return std::make_unique<CcfCommand>();
        case 0xD9: return std::make_unique<RetiCommand>();
        case 0xE0: case 0xF0: return std::make_unique<LdhCommand>();
        case 0xE2: case 0xF2: return std::make_unique<LdhCommand>();
        case 0xE8: return std::make_unique<AddSPCommand>();
        case 0xE9: return std::make_unique<JphlCommand>();
        case 0xEA: case 0xFA: return std::make_unique<LdSpecialCommand>();
        case 0xF3: return std::make_unique<DiCommand>();
        case 0xF8: return std::make_unique<LdSpecialCommand>(); // LD HL, SP+e
        case 0xF9: return std::make_unique<LdSpecialCommand>(); // LD SP, HL
        case 0xFB: return std::make_unique<EiCommand>();
    }

    return nullptr; // Unimplemented opcode
}
