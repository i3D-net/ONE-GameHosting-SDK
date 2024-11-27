# Build, Test and Run scripts for linux

## Build Scripts

The build script provides the following compile options for building the sdk libraries and fake-examples

- Debug or Release Build:
  `_debug` or `_release` options specify whether to build the SDK in debug or release mode.

- Static or Dynamic Library:
  `_a` or `_so` options specify whether to build the SDK as a static library (.a) or a dynamic library (.so).

For example:

`tools\linux\build_debug_a.sh` Builds the SDK as a static library in debug mode.

## Clean Script

`tools\linux\clean.sh` Cleans the build folder. Recommended before running a different build script.

## Test Scripts

Several tests-cases and test-collections are available after a successful build of the SDK as a **static** library.
Test scripts are provided for the three most common used collections of tests.

- short test
    `tools\linux\test_short.sh`

- long test (continuous test maintaining connection)
    `tools\linux\test_long.sh`

- soak test (very high usage over a very long period on multiple threads)
    `tools\linux\test_soak.sh`

Its also possible to run single / individual tests or other test groups.
After compilation the `tests` executable is available in the `build\bin\` directory.

`tests -?` displays help

`test -l` lists all test cases

`test hello` runs a single test case (e.g. hello test)

## Run Scripts

The run script provide a quick test execution of the compiled fake/example executables from the tools folders.
After a build the actual fake/example executables are located in the `build\bin` directory.

For more information about the fake/examples see: [one/fake](/one/fake/readme.md)
