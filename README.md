# Odyssey Game

A PlayStation 4 plugin template using the ORBIS SDK with ImGui rendering capabilities.

## Overview

This project demonstrates the integration of `liborbisrender`, `liborbisutil`, and ImGui for creating interactive homebrew applications on PlayStation consoles. It provides a clean foundation for building custom PS4 applications with graphical user interfaces.

## Prerequisite
- [direct-memory-plugin](https://github.com/BoosieBois/direct-memory-plugin) - needed for games that will not have enough direct memory to allocate the renderer.

## Features

- **ImGui Integration**: Native ImGui rendering support for building intuitive user interfaces
- **Render Hook System**: Flexible hooking library using MinHook
- **Input Handling**: Built-in gamepad input support with customizable callbacks
- **On-Screen Keyboard**: Support for text input via ImGui
- **Frame Management**: Proper frame synchronization and flip handling

## Project Structure

```
odyssey-game/
├── odyssey-game/          # Main application source
│   ├── app.cpp           # Application implementation
│   ├── app.h             # Application header
│   ├── prx.cpp           # PRX entry point
│   └── src/              # Additional source files
├── dependencies/          # Third-party libraries
│   ├── liborbisrender/   # PlayStation rendering library
│   ├── liborbisutil/     # PlayStation utilities
│   ├── minhook/          # Function hooking library
│   ├── fmtlib/           # String formatting library
│   └── StubMaker/        # SDK stub generation tools
└── ORBIS_Debug/          # Build output directory
└── ORBIS_Release/          # Build output directory
```

## Dependencies

- **liborbisrender**: Core rendering and ImGui integration
- **liborbisutil**: PlayStation system utilities (threading, input, logging)
- **minhook**: Function hooking framework
- **fmtlib**: Modern C++ string formatting library
- **ORBIS SDK**: PlayStation 4 development SDK

## Building

### Prerequisites

- Visual Studio 2017 or later
- ORBIS SDK properly installed and configured
- PlayStation console with homebrew support (GoldHEN recommended)

### Build Steps

1. Open `odyssey-game.sln` in Visual Studio
2. Select the desired configuration (Debug|ORBIS or Release|ORBIS)
3. Build the solution (Ctrl+Shift+B)
4. The compiled `.prx` and `.sprx` files will be output to the `ORBIS_Debug/` or `ORBIS_Release/` directory

## Usage

### Installation

1. Transfer the compiled `.sprx` file to your PlayStation console
2. Load the plugin using your homebrew loader (e.g., GoldHEN's plugin system)

### Controls

- **Touchpad (2 fingers)**: Toggle ImGui overlay visibility
- Additional controls can be customized via the gamepad input callback

### Configuration

The application supports various render flags that can be configured in `prx.cpp`:

- `FunctionImGui`: Enable ImGui rendering
- `HookFlip`: Hook into `sceGnmSubmitAndFlipCommandBuffers`
- `HookFlipForWorkload`: Hook into `sceGnmSubmitAndFlipCommandBufferForWorkload`
- `HookFlipVideoOut`: Hook into `sceVideoOutSubmitFlip`
- `FunctionRenderDebug`: Display debug information (requires FunctionImGui)
- `UnlockFps`: Uncap framerate (may cause screen tearing)
- `SubmitSelf`: Call `sce::Gnm::submitDone` after each frame
- `FlipModeVSync`: Force flipmode to SCE_VIDEO_OUT_FLIP_MODE_VSYNC preventing flickering
- `RenderBeforeFlip`: Execute render callback before flip

## Development

### Creating Custom UI

Modify the `run_gui()` function in `prx.cpp` to add your custom ImGui interface:

```cpp
void run_gui()
{
    if (!app.is_open)
        return;

    ImGui::Begin("Your Window Title");
    // Add your ImGui widgets here
    ImGui::Text("Hello, World!");
    ImGui::End();
}
```

### Render Callback

The `render()` function is called every frame:

```cpp
void render(int flipIndex)
{
    if (app.update(flipIndex))
    {
        run_gui();
        app.render(flipIndex);
    }
}
```

## License

This project is provided as-is for educational and development purposes. Ensure compliance with all applicable laws and platform terms of service when using this software.

## Acknowledgments

- ImGui by Omar Cornut
- GoldHEN team for homebrew enablement
- The Boosie Bois
- All dependency library authors

## Support

For issues, questions, or contributions, please refer to the project's GitHub repository or relevant homebrew development communities.
