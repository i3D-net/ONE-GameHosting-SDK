set(HEADER_FILES
    allocator.h
    c_api.h
    c_error.h
    c_platform.h
    error.h
    internal/pinger.h
    internal/sites_endpoint.h
    internal/site_information.h
    internal/udp_socket.h
    internal/version.h
    ip_list.h
    logger.h
    pingers.h
    sites_getter.h
    types.h
)

set(SOURCE_FILES
    allocator.cpp
    c_api.cpp
    c_error.cpp
    error.cpp
    internal/pinger.cpp
    internal/sites_endpoint.cpp
    internal/site_information.cpp
    internal/udp_socket.cpp
    ip_list.cpp
    pingers.cpp
    sites_getter.cpp
    types.cpp
)

option(SHARED_ARCUS_LIB "Build the Arcus library as a shared library" OFF)
if(SHARED_ARCUS_LIB)
    message("Configuring arcus as shared library")
    add_library(one_ping SHARED ${SOURCE_FILES} ${HEADER_FILES})
else()
    message("Configuring arcus as static library")
    add_library(one_ping ${SOURCE_FILES} ${HEADER_FILES})
endif()

include_directories(${PROJECT_SOURCE_DIR})

target_include_directories(one_ping PUBLIC .)

target_compile_features(one_ping PUBLIC cxx_std_11)

if(WIN32)
    target_link_libraries(one_ping wsock32 ws2_32)
endif()
