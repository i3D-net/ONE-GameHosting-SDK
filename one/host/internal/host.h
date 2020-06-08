#pragma once

namespace one {

class Host final {
public:
    // Lifecycle.
    Host();
    ~Host();

    // Connection management.
    int listen();
};

}  // namespace one
