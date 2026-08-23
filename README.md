# ZooEngine

A from-scratch, low-poly 3D engine and Zoo Tycoon-style game, built solo in C++. See [ENGINE_PLAN.md](ENGINE_PLAN.md) for the full build plan, phases, and current progress.

## Current state

**Phase 0 — Skeleton, Sprint 1 (Window and loop), in progress.**

What's implemented so far:

- SDL3 window + 2D renderer, vendored via CMake `FetchContent` (pinned to `release-3.4.14`)
- Fixed-timestep simulation loop with an accumulator (60 ticks/sec), decoupled from render rate, with interpolated rendering (`alpha`) between ticks
- Rolling average frame-time tracking (100-frame ring buffer) and derived FPS
- Dear ImGui vendored via `FetchContent` (pinned to `v1.92.9b`), using the SDL3 + SDLRenderer3 backend, driving a debug overlay that shows frame ms, FPS, and simulation update count

Right now the "game" is a single window that cycles through a smoothly animated background colour, with the debug overlay on top — this is scaffolding to prove the timing and rendering plumbing works before any real rendering (Phase 1) begins.

## Prerequisites

- **Windows 10/11** (the CMake presets in this repo are Windows-only; SDL3 also supports Linux/macOS but you'd need to configure CMake manually without the provided presets)
- **Visual Studio 2022** or newer, with the **"Desktop development with C++"** workload installed (this provides the MSVC compiler, the Windows SDK, and CMake/Ninja bundled with the IDE)
- **Git**

No other dependencies need installing — SDL3 and Dear ImGui are fetched automatically by CMake during configuration.

## Building from a clean clone

1. **Clone the repo**

    ```bash
    git clone <repo-url> ZooEngine
    cd ZooEngine
    ```

2. **Open a Visual Studio Developer Command Prompt (or PowerShell)**

   The build requires the MSVC compiler environment to be set up (so `cl.exe` and the Windows SDK headers are on the path). Easiest way:

   - Start Menu → "Developer Command Prompt for VS 2022" (or "Developer PowerShell for VS 2022"), **or**
   - From a normal terminal, run:

     ```bash
     "C:\Program Files\Microsoft Visual Studio\<version>\Community\Common7\Tools\VsDevCmd.bat" -arch=x64
     ```

     (adjust the path/edition — Community/Professional/Enterprise — to match your install)

3. **Configure the project** (this also downloads and configures SDL3 and Dear ImGui — takes a few minutes the first time)

    ```bash
    cmake --preset x64-debug
    ```

4. **Build**

    ```bash
    cmake --build out/build/x64-debug --config Debug
    ```

5. **Run**

    ```bash
    out\build\x64-debug\ZooGame.exe
    ```

   You should see a window titled "ZooEngine" with an animated background colour and an ImGui "Debug Overlay" panel showing live frame timing stats.

## Project layout

```
CMakeLists.txt       Root build config — fetches SDL3 + Dear ImGui, defines targets
CMakePresets.json     CMake presets for x64/x86 debug/release (Windows, Ninja, MSVC)
game/main.cpp         Entry point — window, event loop, sim loop, rendering, ImGui overlay
src/                  ZooEngine static library (currently empty — engine code moves here as it grows)
include/              Public headers for the ZooEngine library
ENGINE_PLAN.md        The sprint-by-sprint build plan
```

## Other build configurations

Release build:

```bash
cmake --preset x64-release
cmake --build out/build/x64-release --config Release
```

32-bit presets (`x86-debug`, `x86-release`) also exist but are untested/unused for this project.
