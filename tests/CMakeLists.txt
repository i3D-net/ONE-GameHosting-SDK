set(UNIT_TEST tests)

set(HEADER_FILES
    one/arcus/harness.h
    one/arcus/util.h
)

set(SOURCE_FILES
    one/arcus/harness.cpp
    one/arcus/util.cpp
)

add_library(one_tests ${SOURCE_FILES} ${HEADER_FILES})

# Shared library mode has limited tests since only the exported dll surface is
# available. This could be improved in the future by providing a dll interface
# for the Arcus Client so the Agent can be used in shared library mode.
if(NOT SHARED_ARCUS_LIB)
    target_link_libraries(one_tests PRIVATE one_agent)
    target_link_libraries(one_tests PRIVATE one_game)
    target_link_libraries(one_tests PRIVATE one_ping)
endif()

if(SHARED_ARCUS_LIB)
    set(SOURCE_TEST_FILES
        main.cpp
        one/arcus/shared_lib.cpp
    )
else()
    set(SOURCE_TEST_FILES
        main.cpp
        one/arcus/accumulator.cpp
        one/arcus/allocator.cpp
        one/arcus/agent.cpp
        one/arcus/api.cpp
        one/arcus/array.cpp
        one/arcus/arcus.cpp
        one/arcus/chaos.cpp
        one/arcus/codec.cpp
        one/arcus/concurrency.cpp
        one/arcus/endian.cpp
        one/arcus/error.cpp
        one/arcus/game.cpp
        one/arcus/integration.cpp
        one/arcus/message.cpp
        one/arcus/object.cpp
        one/arcus/parsing.cpp
        one/arcus/ring.cpp
        one/arcus/stress.cpp
        one/ping/http.cpp
        one/ping/icmp_socket.cpp
        one/ping/pinger.cpp
        one/ping/pingers.cpp
        one/ping/site_information.cpp
        one/ping/sites_endpoint.cpp
        one/ping/sites_getter.cpp
    )
endif()

add_executable(${UNIT_TEST} ${SOURCE_TEST_FILES})

find_package(Threads REQUIRED)

if(WIN32)
set(CMAKE_USE_SCHANNEL ON)
endif()

set(CURL_DISABLE_TESTS ON)
set(BUILD_CURL_EXE OFF)
add_subdirectory(curl)

target_compile_features(${UNIT_TEST} PRIVATE cxx_std_11)
target_link_libraries(${UNIT_TEST} PRIVATE one_arcus)
target_link_libraries(${UNIT_TEST} PRIVATE one_ping)
target_link_libraries(${UNIT_TEST} PRIVATE one_tests)
target_link_libraries(${UNIT_TEST} PRIVATE Threads::Threads)
target_link_libraries(${UNIT_TEST} PRIVATE libcurl)

if(WIN32)
target_link_libraries(${UNIT_TEST} PRIVATE winmm.lib)
endif()

include_directories(${PROJECT_SOURCE_DIR}/tests/catch2)
include_directories(${PROJECT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME ${UNIT_TEST} COMMAND ${UNIT_TEST})

if (RUN_TEST_AFTER_BUILD)
    add_custom_command(
        TARGET ${UNIT_TEST}
        COMMENT "Run tests"
        POST_BUILD
        // -d yes is for showing durations of test
        // the exclude filter avoids running long tests for each build
        COMMAND ${UNIT_TEST} ~[.] ~long:* ~soak:* ~soak-days:*
    )
endif()
