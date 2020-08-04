add_executable(agent main.cpp)
target_compile_features(agent PRIVATE cxx_std_11)

include_directories(${PROJECT_SOURCE_DIR})

set(HEADER_FILES
    agent.h
)

set(SOURCE_FILES
    agent.cpp
)

add_library(one_agent ${SOURCE_FILES} ${HEADER_FILES})
target_include_directories(one_agent PUBLIC .)