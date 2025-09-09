//
// Created by Jack_ on 31/08/2025.
//

#include "bus.h"
#include "../Utility/utility.h"
#include "cartridgeLoader.h"

void bus::connectComponents(const std::shared_ptr<cartridgeLoader>& loader, const std::shared_ptr<apu>& apu, const std::shared_ptr<joypad>& joypad, const std::shared_ptr<ppu>& ppu, const std::shared_ptr<timer>& timer)
{
	m_cartridge = loader;
	m_apu = apu;
	m_joypad = joypad;
	m_ppu = ppu;
	m_timer = timer;
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

	}
	else if (address < 0xC000)
	{
		//Cartridge RAM
		return m_cartridge->read(address);
	}
	else if (address < 0xE000)
	{
		//Working RAM

	}
	else if (address < 0xFE00)
	{
		//reserved echo ram
		return 0;

	}
	else if (address < 0xFEA0)
	{
		//DAM
		return 0x0;
	}
	else if (address < 0xFF00)
	{
		//reserved memory
		return 0;
	}
	else if (address < 0xFF80)
	{
		//IO
		return 0;
	}
	else if (address < 0xFFFF)
	{
		//CPU Interrupt enable register
		//m_cpu->getIERegister();
	}
	if (utility::inRange(address, 0xC000, 0xCFFF))
	{
		return wram.at(address - 0xC000);
	}
	else if (utility::inRange(address, 0xFF80, 0xFFFE))
	{
		return hram.at(address - 0xFF80);
	}
	else
	{
		LOG_ERROR("invalid memory read");
		return 0;
	}
	return 0;
}

void bus::write(uint16_t address, uint8_t value)
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
		//m_memory->write(address, value);
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
		//m_cpu->setIERegister(value);
	}
	else
	{
		if (utility::inRange(address, 0xC000, 0xCFFF))
		{
			wram.at(address - 0xC000) = value;
			return;
		}
		else if (utility::inRange(address, 0xFF80, 0xFFFE))
		{
			hram.at(address - 0xFF80) = value;
			return;
		}
		else
		{
			LOG_ERROR("invalid memory read");
			return;
		}
	}
}
