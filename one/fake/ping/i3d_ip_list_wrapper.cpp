// Copyright i3D.net, 2021. All Rights Reserved.

#include <one/fake/ping/i3d_ip_list_wrapper.h>

#include <one/ping/c_api.h>

namespace i3d_ping_integration {

I3dIpListWrapper::I3dIpListWrapper() : _list(nullptr) {
    i3d_ping_ip_list_create(&_list);
}

I3dIpListWrapper::~I3dIpListWrapper() {
    i3d_ping_ip_list_destroy(_list);
}

I3dIpListPtr I3dIpListWrapper::list() const {
    return _list;
}

}  // namespace i3d_ping_integration
