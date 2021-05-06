#pragma once

#include <one/ping/types.h>
#include <one/ping/error.h>

namespace i3d {
namespace ping {

class IpList final {
public:
    IpList() = default;
    IpList(const IpList &) = default;
    IpList(const Vector<String> &ips);
    IpList &operator=(const Vector<String> &ips);
    IpList &operator=(const IpList &) = default;
    ~IpList() = default;

    const Vector<String> &ips() const;

    void clear();
    void push_back(const String &ip);
    void size(size_t &size);
    I3dPingError ip(unsigned int pos, String &ip) const;

private:
    Vector<String> _ips;
};

}  // namespace ping
}  // namespace i3d