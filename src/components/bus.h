//
// Created by Jack_ on 31/08/2025.
//

#ifndef GAMEBOYEMULATOR_BUS_H
#define GAMEBOYEMULATOR_BUS_H
#include "../interfaces/IComponentMessanger.h"


class cpu;
class timer;
class ppu;
class joypad;
class cartridgeLoader;
class interruptController;

class bus :   public IComponentMessanger
{
public:
	bus()= default;
	void connectComponents(const std::shared_ptr<cartridgeLoader>& loader, const std::shared_ptr<joypad>& joypad, const std::shared_ptr<ppu>& ppu, const std::shared_ptr<timer>& timer, const std::shared_ptr<cpu>& cpu, const std::shared_ptr<interruptController>& interruptController);

	uint8_t read(uint16_t address) override;
	void write(uint16_t address, uint8_t value) override;

	uint8_t readWRam(uint16_t address);
	void writeWRam(uint16_t address, uint8_t value);

	uint8_t readHRam(uint16_t address);
	void writeHRam(uint16_t address, uint8_t value);

	uint8_t readIO(uint16_t address);
	void writeIO(uint16_t address, uint8_t value);

	// Bus-side serial logging ("Serial:" lines + Failed-detection trap).
	// Off by default; the CPU's SerialPortDebugger handles the SERIAL:/RESULT: logs.
	void setSerialLogging(bool enabled) { m_serialLogging = enabled; }
	bool getSerialLogging() const { return m_serialLogging; }

	// Complete serial output, captured passively for the debug panel.
	const char* getSerialOutput() const { return m_serialOutput.data(); }
	void clearSerialOutput() { m_serialOutput[0] = '\0'; m_serialOutputLen = 0; }

private:
	// Non-owning: the emulator is the sole owner of every component and
	// outlives the bus. The bus only uses these, it does not keep them alive.
	cartridgeLoader* m_cartridge = nullptr;
	joypad* m_joypad = nullptr;
	ppu* m_ppu = nullptr;
	timer* m_timer = nullptr;
	interruptController* m_interruptController = nullptr;
	std::weak_ptr<cpu> m_cpu;   // weak: only used by the off-by-default serial debug trap

	std::array<uint8_t, 0x2000> wRam{};
	std::array<uint8_t, 0x80> hRam{};

	std::array<char,2> serialData = {};
	std::string m_serialBuffer;
	bool m_serialLogging = false;

	std::array<char, 8192> m_serialOutput = {0};
	int  m_serialOutputLen = 0;
};


#endif //GAMEBOYEMULATOR_BUS_H