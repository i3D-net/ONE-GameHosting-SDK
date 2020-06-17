add_executable(agent main.cpp)
target_compile_features(agent PRIVATE cxx_std_11)

include_directories(${PROJECT_BINARY_DIR})

set(HEADER_FILES
    client.h
)

set(SOURCE_FILES
    client.cpp
)

add_library(one_agent ${SOURCE_FILES} ${HEADER_FILES})