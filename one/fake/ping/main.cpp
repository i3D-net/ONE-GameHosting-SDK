#include <one/fake/ping/ping.h>

#include <one/fake/ping/log.h>

#include <chrono>
#include <thread>

using namespace i3d_ping_integration;

void sleep(int ms) {
    std::this_thread::sleep_for(milliseconds(ms));
}

int main(int argc, char **argv) {
    Ping ping;
    return 0;
}
