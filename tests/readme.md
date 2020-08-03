# One Game Hosting SDK Tests

All tests for the SDK are contained and managed here.

## Catch2

The excellent external Catch2 testing framework is used.

https://github.com/catchorg/Catch2

tag: `v2.12.2`
Commit: `b1b5cb812277f367387844aab46eb2d3b15d03cd`

## Adding Tests

1. Add tests to the file matching their corresponding component folder within the one folder. For example if adding a test for something within one/arcus/.. then add the test to tests/arcus.cpp. If the test uses multiple components, add it to tests/integration.cpp.
2. Integration tests should only reference public interfaces. No friend classes for testing purposes or touching of /internal folders.
3. If adding a new file, add the reference in the `CMakeList.txt` in the current folder:
    ```cmake
    add_executable(${UNIT_TEST}
        main.cpp
        ...
        NEW_TEST.cpp
    )
    ```
4. Test utilities go in tests/util.h.