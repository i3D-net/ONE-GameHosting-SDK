set(HEADER_FILES
    arcus.h
    internal/socket.h
)

set(SOURCE_FILES
    c_api.cpp
    internal/socket.cpp
)

add_library(one_arcus ${SOURCE_FILES} ${HEADER_FILES})

include_directories(${PROJECT_BINARY_DIR})

target_include_directories(one_arcus PUBLIC .)

target_compile_features(one_arcus PUBLIC cxx_std_11)
