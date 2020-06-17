set(UNIT_TEST tests)

add_executable(${UNIT_TEST}
    main.cpp
    unit/factorial.cpp
    unit/simple.cpp
    unit/agent.cpp
)

target_compile_features(${UNIT_TEST} PRIVATE cxx_std_11)
include_directories(catch2)

add_test(NAME ${UNIT_TEST} COMMAND ${UNIT_TEST})

if (RUN_TEST_AFTER_BUILD)
    add_custom_command(
        TARGET ${UNIT_TEST}
        COMMENT "Run tests"
        POST_BUILD
        COMMAND ${UNIT_TEST}
    )
endif()
