set(HEADER_FILES
    allocator.h
    array.h
    client.h
    c_api.h
    c_error.h
    error.h
    internal/accumulator.h
    internal/codec.h
    internal/connection.h
    internal/endian.h
    internal/health.h
    internal/message.h
    internal/mutex.h
    internal/platform.h
    internal/ring.h
    internal/socket.h
    internal/time.h
    internal/version.h
    message.h
    opcode.h
    object.h
    server.h
    types.h
)

set(SOURCE_FILES
    allocator.cpp
    array.cpp
    client.cpp
    c_api.cpp
    c_error.cpp
    error.cpp
    internal/accumulator.cpp
    internal/codec.cpp
    internal/connection.cpp
    internal/endian.cpp
    internal/health.cpp
    internal/message.cpp
    internal/socket.cpp
    internal/time.cpp
    message.cpp
    object.cpp
    server.cpp
)

add_library(one_arcus ${SOURCE_FILES} ${HEADER_FILES})

include_directories(${PROJECT_SOURCE_DIR})

target_include_directories(one_arcus PUBLIC .)

target_compile_features(one_arcus PUBLIC cxx_std_11)

if(WIN32)
    target_link_libraries(one_arcus wsock32 ws2_32)
endif()
