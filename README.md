# GameBoy Emulator

A work-in-progress Game Boy (DMG) emulator written in C++20, using **SDL3** for the
window and **Dear ImGui** for the debug UI.

## Status

Work in progress:

- ✅ **CPU** — passes all of Blargg's `cpu_instrs` (01–11) plus the timer/interrupt test
- ✅ **Timer & interrupts** — DIV/TIMA, IF/IE and interrupt dispatch working
- 🚧 **PPU** — VRAM/OAM, LCD registers and scanline timing work; on-screen pixel rendering, cycle-accurate timing and OAM DMA are still being finished
- 🛠️ **Debug UI** — CPU, bus, cartridge and VRAM tile viewers
- ❌ **Audio (APU)** — not started

## Prerequisites

- **CMake** (4.0+, as set in `CMakeLists.txt`)
- A **C++20 compiler** (MSVC 2022 on Windows; GCC or Clang elsewhere)
- **Git** + an internet connection for the first build

You don't need to install any libraries yourself. SDL3, SDL3_ttf, Dear ImGui,
spdlog, Native File Dialog, nlohmann/json and GoogleTest are all downloaded
automatically by CMake the first time you configure the project.

## Build

```sh
git clone <repo-url>
cd GBEmulator
cmake -S . -B build      # first run downloads the dependencies (needs internet)
cmake --build build
```

The app is then at `build/src/GameBoyEmulator` (`.exe` on Windows). Required DLLs
are copied next to it automatically.

> Using CLion or Visual Studio? Just open the project folder — it configures itself.

## Run

```sh
./build/src/GameBoyEmulator
```

Pick a `.gb` ROM from the file dialog, then:

| Key | Action |
|-----|--------|
| `` ` `` (backtick) | Toggle the debug UI (CPU, bus, cartridge, VRAM tiles) |
| `Space` | Pause / resume |
| `Esc` | Quit |

## Running test ROMs (headless)

`romrunner` runs a ROM with no UI and prints its serial output (e.g. Blargg's
`Passed` / `Failed #N`). Handy for automated checks:

```sh
cmake --build build --target romrunner
./build/tests/romrunner "roms/02-interrupts.gb"
```

## References

- https://meganesu.github.io/generate-gb-opcodes/
- https://gekkio.fi/files/gb-docs/gbctr.pdf
- https://petar-v.com/talks/emulation.pdf
- https://github.com/SingleStepTests/sm83
