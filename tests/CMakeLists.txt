set(UNIT_TEST tests)

set(HEADER_FILES
    harness.h
    util.h
)

set(SOURCE_FILES
    harness.cpp
    util.cpp
)

add_library(one_tests ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(one_tests PRIVATE one_agent)
target_link_libraries(one_tests PRIVATE one_game)
target_link_libraries(one_tests PRIVATE one_arcus)

add_executable(${UNIT_TEST}
    allocator.cpp
    accumulator.cpp
    agent.cpp
    array.cpp
    arcus.cpp
    chaos.cpp
    codec.cpp
    concurrency.cpp
    endian.cpp
    error.cpp
    game.cpp
    integration.cpp
    main.cpp
    message.cpp
    object.cpp
    parsing.cpp
    ring.cpp
    stress.cpp
)

find_package(Threads REQUIRED)

target_compile_features(${UNIT_TEST} PRIVATE cxx_std_11)
target_link_libraries(${UNIT_TEST} PRIVATE one_agent)
target_link_libraries(${UNIT_TEST} PRIVATE one_game)
target_link_libraries(${UNIT_TEST} PRIVATE one_arcus)
target_link_libraries(${UNIT_TEST} PRIVATE one_tests)
target_link_libraries(${UNIT_TEST} PRIVATE Threads::Threads)
if(WIN32)
target_link_libraries(${UNIT_TEST} PRIVATE winmm.lib)
endif()

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
        COMMAND ${UNIT_TEST} ~long:* ~soak:*
    )
endif()
