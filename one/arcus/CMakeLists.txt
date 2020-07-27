set(HEADER_FILES
    arcus.h
    message.h
    internal/codec.h
    internal/connection.h
    internal/platform.h
    internal/ring.h
    internal/socket.h
)

set(SOURCE_FILES
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

if(WIN32)
    target_link_libraries(one_arcus wsock32 ws2_32)
endif()
