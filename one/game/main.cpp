#include <iostream>
#include <version.h>

#include <c_api.h>

int main() {
    std::cout << "Game -> project name: " << ONE_NAME << " version: " << ONE_VERSION << std::endl;
    OneGameHostApiPtr api = one_game_host_api();
    auto host = api->create_client();
    api->destroy_client(host);
    return 0;
}