#include <iostream>
#include <version.h>

#include <c_api.h>

int main() {
    std::cout << "Game -> project name: " << ONE_NAME << " version: " << ONE_VERSION << std::endl;
    auto server = one_server_create();
    one_server_destroy(server);
    return 0;
}