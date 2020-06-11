set(HEADER_FILES
    c_api.h
    internal/host.h
    internal/types.h
)

set(SOURCE_FILES
    c_api.cpp
    internal/host.cpp
)

add_library(one_host ${SOURCE_FILES} ${HEADER_FILES})

include_directories(${PROJECT_BINARY_DIR})

target_include_directories(one_host PUBLIC .)

target_compile_features(one_host PUBLIC cxx_std_11)
