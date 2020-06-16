# Catch2

https://github.com/catchorg/Catch2

tag: `v2.12.2`
Commit: `b1b5cb812277f367387844aab46eb2d3b15d03cd`

# Adding Unit Tests

To add an unit test:
1. add the corresponding source in the `unit` folder
2. add the reference in the `CMakeList.txt` in this current folder

```cmake
add_executable(${UNIT_TEST}
    main.cpp
    ...
    unit/NEW_TEST.cpp
)
```