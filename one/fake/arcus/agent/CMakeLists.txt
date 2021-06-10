# Agent executable should not build in shared library mode as it uses
# unexported Arcus library symbols.
if(SHARED_ARCUS_LIB)
    return()
endif()

add_executable(agent main.cpp)
target_compile_features(agent PRIVATE cxx_std_11)

target_link_libraries(agent PRIVATE one_agent one_game one_arcus)

include_directories(${PROJECT_SOURCE_DIR})

set(HEADER_FILES
    agent.h
    log.h
)

set(SOURCE_FILES
    agent.cpp
    log.cpp
)

add_library(one_agent ${SOURCE_FILES} ${HEADER_FILES})
target_include_directories(one_agent PUBLIC .)
target_link_libraries(one_agent PRIVATE one_game one_arcus)
