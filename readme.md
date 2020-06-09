# One Game Hosting SDK

Providing game servers the ability to communicate with the i3D One Platform.

## Documentation

Follow the build instructions to generate documentation in the /docs folder.

## Requirements

1. Doxygen.
    - [Windows](https://www.doxygen.nl/manual/install.html#install_bin_windows)
    - Ubuntu: `sudo apt-get install doxygen`
2. CMake.
3. C++ toolchain. Currently tested environments:
    - Windows 10 Pro w/ VS2017
    - Ubuntu 18.04

## Build

### Configure the build output and initialize cmake

```bash
cmake -s . -B ./build
```

### Build and run tests

Windows:

```bash
cmake --build ./build --config Release --target ALL_BUILD
```

Linux:

```bash
cmake --build ./build --config Release --target all
```

The doxygen documentation is built automatically if cmake is able to find the doxygen installed on your system.

By default, the tests will be run after the build. Set `RUN_TEST_AFTER_BUILD` to false to disable this behavior.

## IDE Support

### VSCode

#### Extensions

Install the plugins:
    1. C/C++ from Microsoft
    2. C++ Intellisense from austin
    3. CMake from twxs
    4. CMake Tools from Microsoft
    5. Clang-format from xaver

#### Settings example

The default `.vscode/settings.json` config is below will help to bootstrap quickly.

```json
{
    "cmake.buildDirectory": "${workspaceFolder}/build",
    "cmake.sourceDirectory": "${workspaceFolder}",
    "editor.formatOnSave": true,
    "C_Cpp.default.configurationProvider": "ms-vscode.cmake-tools",
}
```

#### CMake integration

In order to use CMake directly with `vscode`, see the [cmake tools](https://vector-of-bool.github.io/docs/vscode-cmake-tools/getting_started.html#cmake-tools-quick-start) online documentation.
