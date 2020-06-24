# One Game Hosting SDK Tests

All tests for the SDK is contained and managed here.

## Catch2

The excellent external Catch2 testing framework is used.

https://github.com/catchorg/Catch2

tag: `v2.12.2`
Commit: `b1b5cb812277f367387844aab46eb2d3b15d03cd`

## Adding Tests

1. Add files and tests in the corresponding subfolder: /unit for unit tests testing indivdiual components, /integration for tests that work using game and agents together to test the external API.
2. Add the reference in the `CMakeList.txt` in the current folder:
    ```cmake
    add_executable(${UNIT_TEST}
        main.cpp
        ...
        unit/NEW_TEST.cpp
    )
    ```
