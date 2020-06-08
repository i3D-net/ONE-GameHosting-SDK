set(HEADER_FILES
    c_api.h
    internal/arcus.h
    internal/host.h
    internal/socket.h
    internal/types.h
)

set(SOURCE_FILES
    internal/arcus.cpp
    internal/host.cpp
    internal/socket.cpp
)

add_library(one_host ${SOURCE_FILES} ${HEADER_FILES})

include_directories(${PROJECT_BINARY_DIR})

target_include_directories(one_host PUBLIC .)

target_compile_features(one_host PUBLIC cxx_std_11)
