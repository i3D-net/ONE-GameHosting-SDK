#include <iostream>
#include <version.h>

#include <one/server/c_api.h>

int main() {
    std::cout << "Game -> project name: " << ONE_NAME << " version: " << ONE_VERSION << std::endl;
    OneGameHostingApiPtr api = one_game_hosting_api();
    auto server = api->server_api->create();
    api->server_api->destroy(server);
    return 0;
}