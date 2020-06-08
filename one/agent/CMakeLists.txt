add_executable(agent main.cpp)
target_compile_features(agent PRIVATE cxx_std_11)

include_directories(${PROJECT_BINARY_DIR})

set(HEADER_FILES
    server.h
)

set(SOURCE_FILES
    server.cpp
)