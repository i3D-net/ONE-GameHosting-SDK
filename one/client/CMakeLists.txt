set(HEADER_FILES
    c_api.h
    internal/arcus.h
    internal/client.h
    internal/socket.h
    internal/types.h
)

set(SOURCE_FILES
    internal/arcus.cpp
    internal/client.cpp
    internal/socket.cpp
)

add_library(one_client ${SOURCE_FILES} ${HEADER_FILES})

include_directories(${PROJECT_BINARY_DIR})
include_directories(internal/rapidjson)

target_include_directories(one_client PUBLIC internal)
target_include_directories(one_client PUBLIC .)

target_compile_features(one_client PUBLIC cxx_std_11)
