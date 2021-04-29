set(HEADER_FILES
    allocator.h
    c_api.h
    c_error.h
    c_platform.h
    error.h
    internal/version.h
    logger.h
    types.h
)

set(SOURCE_FILES
    allocator.cpp
    array.cpp
    c_api.cpp
    c_error.cpp
    error.cpp
    types.cpp
)

option(SHARED_ARCUS_LIB "Build the Arcus library as a shared library" OFF)
if(SHARED_ARCUS_LIB)
    message("Configuring arcus as shared library")
    add_library(one_arcus SHARED ${SOURCE_FILES} ${HEADER_FILES})
else()
    message("Configuring arcus as static library")
    add_library(one_arcus ${SOURCE_FILES} ${HEADER_FILES})
endif()

include_directories(${PROJECT_SOURCE_DIR})

target_include_directories(one_arcus PUBLIC .)

target_compile_features(one_arcus PUBLIC cxx_std_11)

if(WIN32)
    target_link_libraries(one_arcus wsock32 ws2_32)
endif()
