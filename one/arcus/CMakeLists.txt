set(HEADER_FILES
    client.h
    c_api.h
    internal/accumulator.h
    internal/codec.h
    internal/connection.h
    internal/message.h
    internal/opcodes.h
    internal/platform.h
    internal/ring.h
    internal/socket.h
    internal/version.h
    message.h
    server.cpp
)

set(SOURCE_FILES
    client.cpp
    c_api.cpp
    internal/accumulator.cpp
    internal/codec.cpp
    internal/connection.cpp
    internal/message.cpp
    internal/socket.cpp
    message.cpp
    server.cpp
)

add_library(one_arcus ${SOURCE_FILES} ${HEADER_FILES})

include_directories(${PROJECT_SOURCE_DIR})

target_include_directories(one_arcus PUBLIC .)

target_compile_features(one_arcus PUBLIC cxx_std_11)

if(WIN32)
    target_link_libraries(one_arcus wsock32 ws2_32)
endif()
