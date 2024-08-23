# VSCode Development Workflow

Setting up VSCode to allow building and testing of the SDK. This is not required for Game Server integration.

## Editor Extensions

1. C/C++ VS2017 toolchain by Microsoft. *Required*
2. C++ Intellisense by austin.
3. CMake by twxs. *Required*
4. CMake Tools by Microsoft. *Required*
    > In order to use CMake directly with `vscode`, see the [cmake tools]> (https://vector-of-bool.github.io/docs/vscode-cmake-tools/getting_started.html#cmake-tools-quick-start) online documentation.

5. Clang-format by xaver.
6. C++ TestMate (Optional, for in-IDE test support).

## Settings

Add the following keys to VSCode user settings.

```json
{
    "cmake.buildDirectory": "${workspaceFolder}/build",
    "cmake.sourceDirectory": "${workspaceFolder}",
    "editor.formatOnSave": true,
    "C_Cpp.default.configurationProvider": "ms-vscode.cmake-tools",
}
```

## Build

1. On the bottom bar, select the appropriate toolchain to build the C++ code in the CMake tools menu.
2. Select the CMake extension from the side bar.
3. Click the "..." button and run:
    - Clean Reconfigure All Projects
    - Build All Projects
