#pragma once

namespace one {

class Server final {
public:
    // Lifecycle.
    Server();
    ~Server();

    // Connection management.
    int listen();
};

}  // namespace one
