add_executable(ping main.cpp)
target_compile_features(ping PRIVATE cxx_std_11)

target_link_libraries(ping PRIVATE one_ping one_ping_wrapper)

include_directories(${PROJECT_SOURCE_DIR})

set(HEADER_FILES
    log.h
    i3d_allocator.h
    i3d_ip_list_wrapper.h
    i3d_pingers_wrapper.h
    i3d_sites_getter_wrapper.h
)

set(SOURCE_FILES
    log.cpp
    i3d_ip_list_wrapper.cpp
    i3d_pingers_wrapper.cpp
    i3d_sites_getter_wrapper.cpp
)

add_library(one_ping_wrapper ${SOURCE_FILES} ${HEADER_FILES})

target_include_directories(one_ping_wrapper PUBLIC .)

target_compile_features(one_ping_wrapper PUBLIC cxx_std_11)

target_link_libraries(one_ping_wrapper PRIVATE one_ping)
