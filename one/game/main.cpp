#include <iostream>
#include <one/arcus/internal/version.h>

#include <one/arcus/c_api.h>

int main() {
    std::cout << "Game -> project name: " << ONE_NAME << " version: " << ONE_VERSION << std::endl;
    OneServerPtr server{nullptr};
    int error = one_server_create(1024, 1024, &server);
    if (error != 0) {
        return error;
    }

    one_server_destroy(&server);
    return 0;
}
