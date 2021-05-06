#include <one/ping/ip_list.h>

namespace i3d {
namespace ping {

IpList::IpList(const Vector<String> &ips) {
    _ips = ips;
}

IpList &IpList::operator=(const Vector<String> &ips) {
    _ips = ips;
    return *this;
}

const Vector<String> &IpList::ips() const {
    return _ips;
}

void IpList::clear() {
    _ips.clear();
}

void IpList::push_back(const String &ip) {
    _ips.push_back(ip);
}

void IpList::size(size_t &size) {
    size = _ips.size();
}

I3dPingError IpList::ip(unsigned int pos, String &ip) const {
    if (_ips.size() <= pos) {
        return I3D_PING_ERROR_IP_LIST_POS_IS_OUT_OF_RANGE;
    }

    ip = _ips[pos];
    return I3D_PING_ERROR_NONE;
}

}  // namespace ping
}  // namespace i3d