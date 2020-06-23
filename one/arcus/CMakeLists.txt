set(HEADER_FILES
    arcus.h
    message.h
    internal/codec.h
    internal/connection.h
    internal/ring.h
    internal/socket.h
)

set(SOURCE_FILES
    c_api.cpp
    arcus.cpp
    message.cpp
    internal/codec.cpp
    internal/connection.cpp
    internal/socket.cpp
)

add_library(one_arcus ${SOURCE_FILES} ${HEADER_FILES})

include_directories(${PROJECT_BINARY_DIR})

target_include_directories(one_arcus PUBLIC .)

target_compile_features(one_arcus PUBLIC cxx_std_11)
