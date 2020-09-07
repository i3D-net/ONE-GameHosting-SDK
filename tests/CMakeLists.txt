set(UNIT_TEST tests)

add_executable(${UNIT_TEST}
    accumulator.cpp
    agent.cpp
    array.cpp
    arcus.cpp
    codec.cpp
    endian.cpp
    error.cpp
    game.cpp
    integration.cpp
    main.cpp
    message.cpp
    object.cpp
    ring.cpp
    util.cpp
)

target_compile_features(${UNIT_TEST} PRIVATE cxx_std_11)
target_link_libraries(${UNIT_TEST} PRIVATE one_agent)
target_link_libraries(${UNIT_TEST} PRIVATE one_game)
target_link_libraries(${UNIT_TEST} PRIVATE one_arcus)
include_directories(catch2)
include_directories(${PROJECT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME ${UNIT_TEST} COMMAND ${UNIT_TEST})

if (RUN_TEST_AFTER_BUILD)
    add_custom_command(
        TARGET ${UNIT_TEST}
        COMMENT "Run tests"
        POST_BUILD
        // -d yes is for showing durations of test
        // the exclude filter avoids running long tests for each build
        COMMAND ${UNIT_TEST} ~long:*
    )
endif()
