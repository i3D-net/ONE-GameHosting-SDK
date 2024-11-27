# VSCode Development Workflow

This Guide explains how to set up VSCode to allow building and testing of the SDK. This is not required for Game Server integration.

See the [build guide for windows](build-windows.md) or [build guide for linux](build-linux.md) for required build tools and compatible c++ compiler.

## VSCode Editor Extensions

1. C/C++ Extension Pack by Microsoft. *Required*
   (includes C++ Intellisense, CMake, CMake Tools)
2. Clang-format by Xaver (optional alternative code formatter).
3. C++ TestMate (Optional, for in-IDE test support).

> In order to use CMake directly with `vscode`, see the [cmake tools]> (<https://vector-of-bool.github.io/docs/vscode-cmake-tools/getting_started.html#cmake-tools-quick-start>) online documentation.

## Settings

Add the following keys to VSCode user settings or workspace settings (.vscode/settings.json).

```json
{
    "cmake.buildDirectory": "${workspaceFolder}/build",
    "cmake.sourceDirectory": "${workspaceFolder}",
    "editor.formatOnSave": true,
    "C_Cpp.default.configurationProvider": "ms-vscode.cmake-tools",
}
```

## Build

There are various options to configure and build the project with VSCode.

### Build from Command Palette (defaults to: Ctrl-Shift-P)

1. > CMake: Select a kit
2. > CMake: Clean (also reconfigures the project)
3. > CMake: Build Target

### Build using CMake Tools in the  status bar

If the bottom status bar does not show CMake Tools, right click on the status and select CMake Tools

1. Select a kit/toolchain e.g. Visual Studio Build Tools 2019
2. Select build variant  (Debug / Release), this will configure the project
3. Select a build target
4. Choose Build

### Build using the SideBar extension

1. Select a kit/toolchain (using one of the above methods)
2. Select the CMake extension from the side bar.
3. From the Project Outline, click the "..." button and choose: Clean Reconfigure All Projects
4. From the Project Outline, choose the Build All Projects icon
