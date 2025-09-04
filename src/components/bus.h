//
// Created by Jack_ on 31/08/2025.
//

#ifndef GAMEBOYEMULATOR_BUS_H
#define GAMEBOYEMULATOR_BUS_H
#include "base/componentMessanger.h"


class timer;
class ppu;
class joypad;
class apu;
class cartridgeLoader;

class bus :   public memorycomponentMessanger
{
public:
	bus()= default;
	void addComponent(const std::string& name, const std::shared_ptr<memoryComponent> &component);
	void connectComponents(const std::shared_ptr<cartridgeLoader>& loader, const std::shared_ptr<apu>& apu, const std::shared_ptr<joypad>& joypad, const std::shared_ptr<ppu>& ppu, const std::shared_ptr<timer>& timer);
	void notify(const std::string& event, std::shared_ptr<memoryComponent> sender) override;

	uint8_t read(uint16_t address) override;
	void write(uint16_t address, uint8_t value) override;
private:
	std::shared_ptr<cartridgeLoader> m_cartridge;
	std::shared_ptr<apu> m_apu;
	std::shared_ptr<joypad> m_joypad;
	std::shared_ptr<ppu> m_ppu;
	std::shared_ptr<timer> m_timer;

	std::array<uint8_t, 0x2000> wram{};
	std::array<uint8_t, 0x7F> hram{};
};


#endif //GAMEBOYEMULATOR_BUS_H