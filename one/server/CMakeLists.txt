set(HEADER_FILES
    c_api.h
    internal/server.h
    internal/types.h
)

set(SOURCE_FILES
    c_api.cpp
    internal/server.cpp
)

add_library(one_server ${SOURCE_FILES} ${HEADER_FILES})

include_directories(${PROJECT_BINARY_DIR})

target_include_directories(one_server PUBLIC .)

target_compile_features(one_server PUBLIC cxx_std_11)
