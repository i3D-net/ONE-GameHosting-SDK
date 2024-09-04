# Build, Test and Run scripts for windows

## Build Scripts

The Buildscript provides the following conmpile options for building the sdk libraries and fake-examples

- Debug or Release Build:
  `_debug` or `_release` options specify whether to build the SDK in debug or release mode.

- Static or Dynamic Library:
  `_lib` or `_dll` options specify whether to build the SDK as a static library (.lib) or a dynamic library (.dll).

- MultiThreaded or MultiThreaded DLL:
  `_mt` or `_mtdll` options specify whether to use the MSVC Multi-Threaded runtime library with static linking or the MSVC Multi-Threaded DLL runtime library with dynamic linking (default).

For example:

`tools\windows\build_debug_lib_mtdll.bat` Builds the SDK as a static library with dynamic linking of the MSVC runtime in debug mode.

## Test Scripts

There are several option for running compiled tests. Test scripts are provided for the three most common used collections of tests.

- short test
    `tools\windows\test_release_short.bat` or `tools\windows\test_debug_short.bat`

- long test (continuous test maintaining connection)
    `tools\windows\test_release_long.bat` or `tools\windows\test_debug_long.bat`

- soak test (very high usage over a very long period on multiple threads)
    `tools\windows\test_release_soak.bat` or `tools\windows\test_debug_soak.bat`

Its also possible to run sing / individual tests.
After compilation the `tests.exe` is available in the `build\bin\Release\` or `build\bin\Debug\` directory.

`tests.exe -?` displays help

`tests.exe -l` lists all test cases

'test.exe hello` runs a single test case (e.g. hello test)

## Run Scripts

The run script provide a quick execution of the compiled fake/example executables from the tools folders.
After a succesfull build the fake executables are locatated in the `build\bin\Release\` or `build\bin\Debug\` directory.

For more information about the fake/examples see: [one/fake](/one/fake/readme.md)
