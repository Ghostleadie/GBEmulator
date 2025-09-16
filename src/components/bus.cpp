//
// Created by Jack_ on 31/08/2025.
//

#include "bus.h"
#include "../Utility/utility.h"
#include "cartridgeLoader.h"
#include "cpu/cpu.h"

void bus::connectComponents(const std::shared_ptr<cartridgeLoader>& loader, const std::shared_ptr<joypad>& joypad, const std::shared_ptr<ppu>& ppu, const std::shared_ptr<timer>& timer, const std::shared_ptr<cpu>& cpu)
{
	m_cartridge = loader;
	m_joypad = joypad;
	m_ppu = ppu;
	m_timer = timer;
	m_cpu = cpu;
}

void bus::notify(const std::string& event, std::shared_ptr<memoryComponent> sender)
{

}

uint8_t bus::read(uint16_t address)
{
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
		LOG_INFO("Bus Read from I/o: {:04X}", address);
		return 0;
	}
	else if (address < 0xFFFF)
	{
		LOG_INFO("Bus Read from Interrupt enable register: {:04X}", address);
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
			LOG_ERROR("invalid memory read: {}", address);
			return 0;
		}
	}
	return 0;
}

void bus::write(uint16_t address, const uint8_t value)
{
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
		//m_io->write(m_cpu, address, value);
	}
	else if (address < 0xFFFF)
	{
		//CPU Interrupt enable register
		m_cpu.lock()->setIERegister(value);
	}
	else
	{
		if (utility::inRange(address, 0xC000, 0xCFFF))
		{
			writeWRam(address,value);
			return;
		}
		else if (utility::inRange(address, 0xFF80, 0xFFFE))
		{
			writeHRam(address,value);
			return;
		}
		else
		{
			LOG_ERROR("invalid memory write: {}", address);
			return;
		}
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
