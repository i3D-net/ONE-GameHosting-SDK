add_executable(game main.cpp)
target_compile_features(game PRIVATE cxx_std_11)

target_link_libraries(game PRIVATE one_game one_arcus)

include_directories(${PROJECT_SOURCE_DIR})

set(HEADER_FILES
    game.h
    log.h
    one_server_wrapper.h
    parsing.h
)

set(SOURCE_FILES
    game.cpp
    log.cpp
    one_server_wrapper.cpp
    parsing.cpp
)

add_library(one_game ${SOURCE_FILES} ${HEADER_FILES})

target_include_directories(one_game PUBLIC .)

target_compile_features(one_game PUBLIC cxx_std_11)

add_custom_command(TARGET one_game
    POST_BUILD
    DEPENDS one_arcus
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:one_arcus>
        $<TARGET_FILE_DIR:one_game>
)
