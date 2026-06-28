//
// Created by Jack_ on 31/08/2025.
//

#include "bus.h"
#include "../Utility/utility.h"
#include "cartridgeLoader.h"
#include "timer.h"
#include "ppu.h"
#include "joypad.h"
#include "cpu/cpu.h"
#include "interruptController.h"
#include <cstring>

void bus::connectComponents(const std::shared_ptr<cartridgeLoader>& loader, const std::shared_ptr<joypad>& joypad, const std::shared_ptr<ppu>& ppu, const std::shared_ptr<timer>& timer, const std::shared_ptr<cpu>& cpu, const std::shared_ptr<interruptController>& interruptController)
{
	m_cartridge = loader.get();
	m_joypad = joypad.get();
	m_ppu = ppu.get();
	if (m_ppu)
	{
		m_ppu->connectBus(this);
	}
	m_timer = timer.get();
	m_cpu = cpu;
	m_interruptController = interruptController.get();
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
		//VRAM
		return m_ppu->read(address);
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
		//echo ram mirrors WRAM (0xC000-0xDDFF)
		return readWRam(address - 0x2000);
	}
	else if (address < 0xFEA0)
	{
		//OAM
		return m_ppu->read(address);
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
		return m_interruptController->read(0xFFFF);
	}
	else
	{
		//High RAM (0xFF80 - 0xFFFE)
		return readHRam(address);
	}
	return 0;
}

void bus::write(uint16_t address, const uint8_t value)
{

if (address == 0xFF01) // Serial data register
    {
        const char c = static_cast<char>(value);

        // Always record the serial stream so the debug panel can show the full
        // output. This is passive (it never touches 0xFF02), so it doesn't race
        // with the CPU's serial-transfer handling the way the old panel did.
        if (m_serialOutputLen >= static_cast<int>(sizeof(m_serialOutput)) - 1)
        {
            // Buffer full: keep the most recent half.
            const int keep = static_cast<int>(sizeof(m_serialOutput)) / 2;
            memmove(m_serialOutput.data(), m_serialOutput.data() + (m_serialOutputLen - keep), keep);
            m_serialOutputLen = keep;
        }
        m_serialOutput[m_serialOutputLen++] = c;
        m_serialOutput[m_serialOutputLen] = '\0';

        // Optional bus-side per-line logging + Failed-detection trap (off by default).
        if (m_serialLogging)
        {
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
		//VRAM
		m_ppu->write(address, value);
		return;
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
		//echo ram mirrors WRAM (0xC000-0xDDFF)
		writeWRam(address - 0x2000, value);
		return;
	}
	else if (address < 0xFEA0)
	{
		//OAM
		if (m_ppu->isTransferringDMA())
		{
			return;
		}
		m_ppu->write(address, value);
		return;
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
		m_interruptController->write(0xFFFF, value);
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
	if (address == 0xFF00)
	{
		return m_joypad->read(address);
	}
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
		return m_interruptController->read(0xFF0F);
	}

	if (utility::inRange(address, 0xFF40, 0xFF4B))
	{
		return m_ppu->read(address);
	}

	return 0xFF;
}

void bus::writeIO(uint16_t address, uint8_t value)
{
	if(address == 0xFF00)
	{
		m_joypad->write(address, value);
		return;
	}
	if (address == 0xFF01)
	{
		serialData[0] = value;
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
		m_interruptController->write(0xFF0F, value);
		return;
	}

	if (utility::inRange(address, 0xFF40, 0xFF4B))
	{
		m_ppu->write(address, value);
		return;
	}
}
