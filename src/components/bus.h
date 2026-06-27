//
// Created by Jack_ on 31/08/2025.
//

#ifndef GAMEBOYEMULATOR_BUS_H
#define GAMEBOYEMULATOR_BUS_H
#include "base/componentMessanger.h"


class cpu;
class timer;
class ppu;
class joypad;
class cartridgeLoader;

class bus :   public memorycomponentMessanger
{
public:
	bus()= default;
	void connectComponents(const std::shared_ptr<cartridgeLoader>& loader, const std::shared_ptr<joypad>& joypad, const std::shared_ptr<ppu>& ppu, const std::shared_ptr<timer>& timer, const std::shared_ptr<cpu>& cpu);

	uint8_t read(uint16_t address) override;
	void write(uint16_t address, uint8_t value) override;

	uint8_t readWRam(uint16_t address);
	void writeWRam(uint16_t address, uint8_t value);

	uint8_t readHRam(uint16_t address);
	void writeHRam(uint16_t address, uint8_t value);

	uint8_t readIO(uint16_t address);
	void writeIO(uint16_t address, uint8_t value);

	// Bus-side serial logging ("Serial:" lines + Failed-detection trap).
	// Off by default; the CPU dbg path handles the SERIAL:/RESULT: logs.
	void setSerialLogging(bool enabled) { m_serialLogging = enabled; }
	bool getSerialLogging() const { return m_serialLogging; }

private:
	std::shared_ptr<cartridgeLoader> m_cartridge;
	std::shared_ptr<joypad> m_joypad;
	std::shared_ptr<ppu> m_ppu;
	std::shared_ptr<timer> m_timer;
	std::weak_ptr<cpu> m_cpu;

	std::array<uint8_t, 0x2000> wRam{};
	std::array<uint8_t, 0x80> hRam{};

	char serialData[2];
	std::string m_serialBuffer;
	bool m_serialLogging = false;
};


#endif //GAMEBOYEMULATOR_BUS_H