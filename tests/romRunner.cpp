//
// Headless Blargg test-ROM runner.
//
// Builds the emulator core (bus/cpu/timer/clock/cartridge/joypad/ppu/interrupt
// controller) with no SDL/UI, runs a ROM, and streams the serial output
// (Blargg "Passed" / "Failed #N"). Stops once a verdict is printed or the
// instruction cap is hit.
//
//   romrunner <rom> [maxInstr]
//

#include <memory>
#include <string>
#include <iostream>
#include <cstring>
#include <cstdint>
#include <cstdlib>

#include "log/log.h"
#include "components/bus.h"
#include "components/cpu/cpu.h"
#include "components/timer.h"
#include "components/ppu.h"
#include "components/joypad.h"
#include "components/cartridgeLoader.h"
#include "components/interruptController.h"
#include "components/emulatorClock.h"

int main(int argc, char** argv)
{
	log::Init();
	// Silence the core's chatty LOG_* output; we print serial ourselves.
	log::GetEmulatorLogger()->set_level(spdlog::level::off);

	if (argc < 2)
	{
		std::cout << "usage: romrunner <rom> [maxInstr]\n";
		return 1;
	}

	const std::string romPath = argv[1];
	const long long maxInstr = (argc >= 3) ? std::atoll(argv[2]) : 50000000LL;

	auto busPtr    = std::make_shared<bus>();
	auto timerPtr  = std::make_shared<timer>();
	auto irqPtr    = std::make_shared<interruptController>();
	auto clockPtr  = std::make_shared<emulatorClock>();
	auto cpuPtr    = std::make_shared<cpu>(busPtr, clockPtr);
	auto cartPtr   = std::make_shared<cartridgeLoader>();
	auto joypadPtr = std::make_shared<joypad>();
	auto ppuPtr    = std::make_shared<ppu>();

	busPtr->connectComponents(cartPtr, joypadPtr, ppuPtr, timerPtr, cpuPtr, irqPtr);
	ppuPtr->connectBus(busPtr.get());
	cpuPtr->init();
	timerPtr->init(irqPtr);
	clockPtr->addDevice(timerPtr.get());
	clockPtr->addDevice(ppuPtr.get());

	if (!cartPtr->loadCartridge(romPath))
	{
		std::cout << "Failed to load ROM: " << romPath << "\n";
		return 1;
	}

	size_t lastLen = 0;
	bool done = false;
	for (long long i = 0; i < maxInstr && !done; ++i)
	{
		cpuPtr->emulateCycle();

		// Cheap-ish: only poll the serial buffer periodically.
		if ((i & 0x3FF) == 0)
		{
			const char* serial = busPtr->getSerialOutput();
			const size_t len = std::strlen(serial);
			if (len != lastLen)
			{
				std::cout << (serial + lastLen);
				std::cout.flush();
				lastLen = len;

				if (std::strstr(serial, "Passed") || std::strstr(serial, "Failed"))
				{
					// Flush any trailing characters, then stop.
					for (int k = 0; k < 200000; ++k) cpuPtr->emulateCycle();
					done = true;
				}
			}
		}
	}

	// Diagnostics: did the ROM get past WaitVBlank and actually write VRAM?
	int vramNonZero = 0;
	for (int i = 0; i < 0x1800; ++i) if (busPtr->read(0x8000 + i) != 0) ++vramNonZero;
	std::cout << "\n[diag] LY(FF44)=" << (int)busPtr->read(0xFF44)
	          << "  LCDC(FF40)=0x" << std::hex << (int)busPtr->read(0xFF40) << std::dec
	          << "  PC=0x" << std::hex << (int)cpuPtr->getPC() << std::dec
	          << "  VRAM non-zero=" << vramNonZero << "/6144\n";

	std::cout << "\n=== FULL SERIAL ===\n" << busPtr->getSerialOutput() << "\n";
	return 0;
}
