add_executable(ping main.cpp)
target_compile_features(ping PRIVATE cxx_std_11)

target_link_libraries(ping PRIVATE one_ping one_ping_wrapper)

include_directories(${PROJECT_SOURCE_DIR})

set(HEADER_FILES
    log.h
    i3d_ping_ip_list_wrapper.h
    i3d_pingers_wrapper.h
    i3d_sites_getter_wrapper.h
    ping.h
)

set(SOURCE_FILES
    log.cpp
    i3d_ping_ip_list_wrapper.cpp
    i3d_pingers_wrapper.cpp
    i3d_sites_getter_wrapper.cpp
    ping.cpp
)

add_library(one_ping_wrapper ${SOURCE_FILES} ${HEADER_FILES})

target_include_directories(one_ping_wrapper PUBLIC .)

target_compile_features(one_ping_wrapper PUBLIC cxx_std_11)

add_custom_command(TARGET one_ping_wrapper
    POST_BUILD
    DEPENDS one_ping
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:one_ping>
        $<TARGET_FILE_DIR:one_ping_wrapper>
)
