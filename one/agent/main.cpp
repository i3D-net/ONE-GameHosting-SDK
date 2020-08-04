#include <iostream>
#include <one/arcus/internal/version.h>

int main() {
    std::cout << "Agent -> project name: " << ONE_NAME << " version: " << ONE_VERSION << std::endl;
    return 0;
}