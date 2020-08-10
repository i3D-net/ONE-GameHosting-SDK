set(UNIT_TEST tests)

add_executable(${UNIT_TEST}
    agent.cpp
    arcus.cpp
    codec.cpp
    game.cpp
    integration.cpp
    main.cpp
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
        COMMAND ${UNIT_TEST}
    )
endif()
