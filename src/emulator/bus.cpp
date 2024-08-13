#include "bus.h"
#include "cartridge.h"
#include "memory.h"
#include "cpu/cpu.h"
#include <type_traits>

bus::bus(std::shared_ptr<cartridgeLoader> loader, std::shared_ptr<memory> memory)
{
	m_loader = loader;
	m_memory = memory;
}

bus::bus(std::shared_ptr <cartridgeLoader> loader, std::shared_ptr <memory> memory, std::shared_ptr <cpu> cpu)
{
	m_loader = loader;
	m_memory = memory;
	m_cpu = cpu;
}

void bus::connectCPU(std::shared_ptr<cpu> cpu)
{
	m_cpu = cpu;
}

uint8_t bus::read(uint16_t address,int bits)
{
	if (bits == 8)
	{
		if (address < 0x8000)
		{
			//ROM Data
			return m_loader->readCartridge(address);
		}
		else if (address < 0xA000)
		{
			//Character map data
			
		}
		else if (address < 0xC000)
		{
			//Cartridge RAM
			return m_loader->readCartridge(address);
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
			return 0x0;
		}
		else if (address < 0xFFFF)
		{
			//CPU Interrupt enable register
			m_cpu->getIERegister();
		}
		return m_memory->read(address);
	}
	else if (bits == 16)
	{
		uint16_t lo = read(address);
		uint16_t hi = read(address + 1);

		return lo | (hi << 8);
	}
}
void bus::write(uint16_t address, uint8_t value)
{
	if (address < 0x8000)
	{
		//ROM Data
		m_loader->writeToCartridge(address, value);
		return;
	}
	else if (address < 0xA000)
	{
		//Character map data

	}
	else if (address < 0xC000)
	{
		//Cartridge RAM
		m_loader->writeToCartridge(address, value);
		return;
	}
	else if (address < 0xE000)
	{
		//Working RAM
		m_memory->write(address, value);
	}
	else if (address < 0xFE00)
	{
		//reserved echo ram
		GBE_ERROR("Trying to write into reserved memory");
		return;
	}
	else if (address < 0xFEA0)
	{
		//DAM

	}
	else if (address < 0xFF00)
	{
		//reserved memory
		GBE_ERROR("Trying to write into reserved memory");
		return;
	}
	else if (address < 0xFF80)
	{
		//IO

	}
	else if (address < 0xFFFF)
	{
		//CPU Interrupt enable register
		m_cpu->setIERegister(value);
	}
	else
	{
		m_memory->write(address, value);
	}
}

void bus::write(uint16_t address, uint16_t value)
{
	write(uint16_t(address + 1), uint8_t((value >> 8) & 0xFF));
	write(uint16_t(address), uint8_t(value & 0xFF));
}
