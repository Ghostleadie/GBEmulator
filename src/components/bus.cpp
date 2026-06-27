//
// Created by Jack_ on 31/08/2025.
//

#include "bus.h"
#include "../Utility/utility.h"
#include "cartridgeLoader.h"
#include "timer.h"
#include "cpu/cpu.h"

void bus::connectComponents(const std::shared_ptr<cartridgeLoader>& loader, const std::shared_ptr<joypad>& joypad, const std::shared_ptr<ppu>& ppu, const std::shared_ptr<timer>& timer, const std::shared_ptr<cpu>& cpu)
{
	m_cartridge = loader;
	m_joypad = joypad;
	m_ppu = ppu;
	m_timer = timer;
	m_cpu = cpu;
}

uint8_t bus::read(uint16_t address)
{

	if (address <= 0x00FF) {
		if (m_cartridge) {
			return m_cartridge->read(address);
		}
		return 0xFF;
	}
	if (address < 0x8000)
	{
		//ROM Data
		return m_cartridge->read(address);
	}
	else if (address < 0xA000)
	{
		//Character map data
		LOG_INFO("Bus Read from Character map data: {:04X}", address);
		return 0;
	}
	else if (address < 0xC000)
	{
		//Cartridge RAM
		return m_cartridge->read(address);
	}
	else if (address < 0xE000)
	{
		//Working RAM
		return readWRam(address);
	}
	else if (address < 0xFE00)
	{
		//reserved echo ram
		LOG_INFO("Bus Read from echo ram: {:04X}", address);
		return 0;

	}
	else if (address < 0xFEA0)
	{
		//DAM
		LOG_INFO("Bus Read from DAM: {:04X}", address);
		return 0x0;
	}
	else if (address < 0xFF00)
	{
		//reserved memory
		LOG_INFO("Bus Read from Reserve memory: {:04X}", address);
		return 0;
	}
	else if (address < 0xFF80)
	{
		//IO
		//LOG_INFO("Bus Read from I/o: {:04X}", address);
		return readIO(address);
	}
	else if (address == 0xFFFF)
	{
		//LOG_INFO("Bus Read from Interrupt enable register: {:04X}", address);
		//CPU Interrupt enable register
		return m_cpu.lock()->getIERegister();
	}
	else
	{
		if (utility::inRange(address, 0xC000, 0xCFFF))
		{
			return readWRam(address);
		}
		else if (utility::inRange(address, 0xFF80, 0xFFFE))
		{
			return readHRam(address);
		}
		else
		{
			LOG_ERROR("invalid memory read: {:04X}", address);
			return 0;
		}
	}
	return 0;
}

void bus::write(uint16_t address, const uint8_t value)
{

if (m_serialLogging && address == 0xFF01) // Serial data register (logging only)
    {
        char c = static_cast<char>(value);
        m_serialBuffer += c;

        if (c == '\n')
        {
            LOG_INFO("Serial: {}", m_serialBuffer);

            // Check for failure
            if (m_serialBuffer.find("Failed") != std::string::npos)
            {
                LOG_ERROR("═══════════════════════════════════════");
                LOG_ERROR("TEST FAILED DETECTED");
                LOG_ERROR("Serial: {}", m_serialBuffer);
                LOG_ERROR("PC: 0x{:04X}", m_cpu.lock()->getRegisters()->pc);
                LOG_ERROR("Last opcode: 0x{:02X}", m_cpu.lock()->getCurrentOpcode());
                LOG_ERROR("A:{:02X} F:{:02X} B:{:02X} C:{:02X} D:{:02X} E:{:02X} H:{:02X} L:{:02X}",
                          m_cpu.lock()->getRegisters()->a, m_cpu.lock()->getRegisters()->f,
                          m_cpu.lock()->getRegisters()->b, m_cpu.lock()->getRegisters()->c,
                          m_cpu.lock()->getRegisters()->d, m_cpu.lock()->getRegisters()->e,
                          m_cpu.lock()->getRegisters()->h, m_cpu.lock()->getRegisters()->l);
                LOG_ERROR("SP:{:04X}", m_cpu.lock()->getRegisters()->sp);
                LOG_ERROR("═══════════════════════════════════════");

                #if defined(_MSC_VER)
                __debugbreak();
                #else
                __builtin_trap();
                #endif
            }

            m_serialBuffer.clear();
        }
    }

	//LOG_INFO("Bus Write: 0x{:02X} to address 0x{:04X}", value, address);
	if (address < 0x8000)
	{
		//ROM Data
		m_cartridge->write(address, value);
		return;
	}
	else if (address < 0xA000)
	{
		//Character map data

	}
	else if (address < 0xC000)
	{
		//Cartridge RAM
		m_cartridge->write(address, value);
		return;
	}
	else if (address < 0xE000)
	{
		//Working RAM
		writeWRam(address, value);
	}
	else if (address < 0xFE00)
	{
		//reserved echo ram
		LOG_ERROR("Trying to write into reserved memory");
		return;
	}
	else if (address < 0xFEA0)
	{
		//DAM

	}
	else if (address < 0xFF00)
	{
		//reserved memory
		LOG_ERROR("Trying to write into reserved memory");
		return;
	}
	else if (address < 0xFF80)
	{
		//IO
		writeIO(address, value);
	}
	else if (address < 0xFFFF)
	{
		//High RAM (0xFF80 - 0xFFFE)
		writeHRam(address, value);
	}
	else
	{
		//CPU Interrupt enable register (0xFFFF)
		m_cpu.lock()->setIERegister(value);
	}
}

uint8_t bus::readWRam(uint16_t address)
{

	address -= 0xC000;

	if (address >= 0x2000) {
		LOG_ERROR("WRam read out of bounds: {}", address);
		exit(-1);
	}

	return wRam[address];
}

void bus::writeWRam(uint16_t address, const uint8_t value)
{
	address -= 0xC000;

	if (address >= 0x2000)
	{
		LOG_ERROR("WRam write out of bounds: {}", address);
		exit(-1);
	}
	wRam[address] = value;
}

uint8_t bus::readHRam(uint16_t address)
{
	address -= 0xFF80;

	return hRam[address];
}

void bus::writeHRam(uint16_t address, const uint8_t value)
{
	address -= 0xFF80;

	hRam[address] = value;
}

uint8_t bus::readIO(uint16_t address)
{
	if (address == 0xFF01)
	{
		return serialData[0];
	}
	if (address == 0xFF02)
	{
		return serialData[1];
	}

	if (utility::inRange(address, 0xFF04, 0xFF07))
	{
		return m_timer->read(address);
	}

	if (address == 0xFF0F)
	{
		return m_cpu.lock()->getInterruptFlags();
	}
	if (address == 0xFF44)
	{
		return 0x90;
	}
	LOG_WARN("Trying to read from IO address other than 0xFF01 & 0xFF02: {:02X}", address);
}

void bus::writeIO(uint16_t address, uint8_t value)
{
	if(address == 0xFF00)
	{
		LOG_INFO("joypad write: 0x{:02X}", value);
	}
	if (address == 0xFF01)
	{
		serialData[0] = value;

	/*	// Enhanced serial logging
		char ch = (value >= 32 && value <= 126) ? (char)value : '?';
		LOG_WARN("=== SERIAL DATA: 0x{:02X} ('{}') ===", value, ch);

		static std::string serialBuffer;
		serialBuffer += ch;
		LOG_WARN("Serial buffer: '{}'", serialBuffer);

		// Check for Blargg test completion
		if (serialBuffer.find("Passed") != std::string::npos ||
			serialBuffer.find("Failed") != std::string::npos ||
			serialBuffer.find("01-special") != std::string::npos) {
			LOG_WARN("=== BLARGG TEST RESULT: {} ===", serialBuffer);
			}
*/
		// When transfer is initiated (bit 7 set), immediately complete it
		/*if (value == 0x81)
		{
			char ch = static_cast<char>(serialData[0]);
			LOG_WARN("=== SERIAL OUT: 0x{:02X} ('{}') ===", serialData[0],
					 (ch >= 32 && ch <= 126) ? ch : '?');

			static std::string serialBuffer;
			serialBuffer += (ch >= 32 && ch <= 126) ? ch : '?';

			if (ch == '\n' || serialBuffer.size() > 100)
			{
				LOG_WARN("Serial output: {}", serialBuffer);
				serialBuffer.clear();
			}

			// Auto-complete the transfer immediately
			serialData[1] = 0x00;

			// Trigger serial interrupt (bit 3)
			m_cpu.lock()->requestInterrupt(interruptTypes::INT_SERIAL);
		}*/
		return;
	}

	if (address == 0xFF02)
	{
		serialData[1] = value;
		//LOG_WARN("=== SERIAL CONTROL: 0x{:02X} ===", value);
		if (value == 0x81) {
			//LOG_WARN("Serial transfer initiated!");
		}
		return;
	}

	if (utility::inRange(address, 0xFF04, 0xFF07))
	{
		m_timer->write(address, value);
		return;
	}

	if (address == 0xFF0F)
	{
		m_cpu.lock()->setInterruptFlags(value);
		return;
	}
}
