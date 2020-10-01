#pragma once

namespace i3d {
namespace one {

// Reverse functionality of std::lock_guard - unlocks the given mutex at
// construction time and then relocks it upon destruction.
template <class T>
class ReverseLockGuard final {
public:
    ReverseLockGuard(T &mutex) : _mutex(mutex) {
        _mutex.unlock();
    }

    ~ReverseLockGuard() {
        _mutex.lock();
    }

    ReverseLockGuard(const ReverseLockGuard &) = delete;
    ReverseLockGuard &operator=(const ReverseLockGuard &) = delete;

private:
    T &_mutex;
};

}  // namespace one
}  // namespace i3d