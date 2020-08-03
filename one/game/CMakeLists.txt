add_executable(game main.cpp)
target_compile_features(game PRIVATE cxx_std_11)

target_link_libraries(game PRIVATE one_arcus)

include_directories(${PROJECT_BINARY_DIR})
include_directories(${PROJECT_SOURCE_DIR})

set(HEADER_FILES
    game.h
)

set(SOURCE_FILES
    game.cpp
)
