# GameBoy Emulator

Game Boy (DMG) emulator. C++20, SDL3 for window, input and audio, Dear ImGui for the debug UI.

## Status

- ✅ **CPU** — passes all of Blargg's `cpu_instrs` (01–11) plus the timer/interrupt test
- ✅ **Timer & interrupts** — DIV/TIMA, IF/IE and interrupt dispatch working
- ✅ **PPU** — VRAM/OAM, LCD registers, scanline pipeline and OAM DMA; background, window and sprites render on-screen with cycle-accurate timing
- 🛠️ **Debug UI** — CPU, bus, cartridge and VRAM tile viewers
- 🎮 **Input** — keyboard & gamepad, rebindable in Settings; saved binds reload live
- ✅ **Audio (APU)** — four channels (2 square, wave, noise), frame sequencer and SDL3 output; master volume in Settings

## Prerequisites

- CMake 4.0+ (set in `CMakeLists.txt`).
- A C++20 compiler. MSVC 2022 on Windows, GCC or Clang elsewhere.
- Git plus an internet connection for the first build.

No libraries to install by hand. CMake pulls SDL3, SDL3_ttf, Dear ImGui, spdlog, Native File Dialog, nlohmann/json and GoogleTest on the first configure.

## Build

```sh
git clone <repo-url>
cd GBEmulator
cmake -S . -B build      # first run downloads dependencies (needs internet)
cmake --build build
```

Binary lands at `build/src/GameBoyEmulator` (`.exe` on Windows). The build copies the DLLs it needs next to it.

> CLion or Visual Studio: open the project folder, it configures itself.

## Run

```sh
./build/src/GameBoyEmulator
```

Pick a `.gb` ROM from the dialog. Keys:

| Key | Action |
|-----|--------|
| `` ` `` (backtick) | Toggle debug UI |
| `Space` | Pause / resume |
| `Esc` | Quit |

Game buttons (D-pad, A, B, Start, Select) and master volume: open the menu bar, then Settings. Rebind keyboard or gamepad there.

## Running test ROMs (headless)

`romrunner` runs a ROM with no UI and prints its serial output (Blargg `Passed` / `Failed #N`). Good for automated checks.

```sh
cmake --build build --target romrunner
./build/tests/romrunner "roms/02-interrupts.gb"
```

## References

- https://gbdev.io/pandocs/
- https://meganesu.github.io/generate-gb-opcodes/
- https://gekkio.fi/files/gb-docs/gbctr.pdf
- https://petar-v.com/talks/emulation.pdf
- https://github.com/SingleStepTests/sm83
