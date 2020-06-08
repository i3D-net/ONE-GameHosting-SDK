#pragma once

namespace one {

class Client final {
public:
    // Lifecycle.
    Client();
    ~Client();

    // Connection management.
    int Listen();
};

}  // namespace one
