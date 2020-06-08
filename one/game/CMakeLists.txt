add_executable(game main.cpp)
target_compile_features(game PRIVATE cxx_std_11)

target_link_libraries(game PRIVATE one_host)

include_directories(${PROJECT_BINARY_DIR})

set(HEADER_FILES
    server.h
)

set(SOURCE_FILES
    server.cpp
)
