add_executable(testlib testlib.cpp)

target_compile_features(testlib PRIVATE cxx_std_11)
include_directories(catch2)

# If you register a test, then ctest and make test will run it.
# You can also run examples and check the output, as well.
add_test(NAME testlibtest COMMAND testlib)

if (RUN_TEST_AFTER_BUILD)
    add_custom_command(
        TARGET testlib
        COMMENT "Run tests"
        POST_BUILD
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMAND ${CMAKE_CTEST_COMMAND} -C $<CONFIGURATION> -R "^testlib" --output-on-failures
    )
endif()
