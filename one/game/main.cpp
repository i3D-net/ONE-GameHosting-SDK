#include <iostream>
#include <version.h>

#include <c_api.h>
#include <internal/client.h>

int main() {
    std::cout << "Game -> project name: " << ONE_NAME << " version: " << ONE_VERSION << std::endl;
    one::Client one;
    return one.Listen();
}