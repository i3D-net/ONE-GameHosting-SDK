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
target_link_libraries(one_tests PRIVATE one_arcus)

# Shared library mode has limited tests since only the exported dll surface is
# available. This could be improved in the future by providing a dll interface
# for the Arcus Client so the Agent can be used in shared library mode.
if(NOT SHARED_ARCUS_LIB)
    target_link_libraries(one_tests PRIVATE one_agent)
    target_link_libraries(one_tests PRIVATE one_game)
endif()

if(SHARED_ARCUS_LIB)
    set(SOURCE_TEST_FILES
        main.cpp
        shared_lib.cpp
    )
else()
    set(SOURCE_TEST_FILES
        accumulator.cpp
        allocator.cpp
        agent.cpp
        api.cpp
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
endif()

add_executable(${UNIT_TEST} ${SOURCE_TEST_FILES})

find_package(Threads REQUIRED)

target_compile_features(${UNIT_TEST} PRIVATE cxx_std_11)
target_link_libraries(${UNIT_TEST} PRIVATE one_tests)
target_link_libraries(${UNIT_TEST} PRIVATE Threads::Threads)
if(WIN32)
target_link_libraries(${UNIT_TEST} PRIVATE winmm.lib)
endif()

include_directories(catch2)
include_directories(${PROJECT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME ${UNIT_TEST} COMMAND ${UNIT_TEST})

add_custom_command(TARGET ${UNIT_TEST}
    POST_BUILD
    DEPENDS one_arcus
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:one_arcus>
        $<TARGET_FILE_DIR:${UNIT_TEST}>
)

if (RUN_TEST_AFTER_BUILD)
    add_custom_command(
        TARGET ${UNIT_TEST}
        COMMENT "Run tests"
        POST_BUILD
        // -d yes is for showing durations of test
        // the exclude filter avoids running long tests for each build
        COMMAND ${UNIT_TEST} ~[.] ~long:* ~soak:* ~soak-days:*
    )
endif()

