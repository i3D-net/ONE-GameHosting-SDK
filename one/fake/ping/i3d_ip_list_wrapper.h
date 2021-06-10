// Copyright i3D.net, 2021. All Rights Reserved.

#pragma once

//------------------------------------------
// One SDK object types forward declarations

struct I3dIpList;
typedef I3dIpList *I3dIpListPtr;

//--------------------------
// C++ wrapping of ping api.

namespace i3d_ping_integration {

class I3dIpListWrapper final {
public:
    I3dIpListWrapper();
    ~I3dIpListWrapper();

    I3dIpListPtr list() const;

protected:
    I3dIpListPtr _list;
};

}  // namespace i3d_ping_integration
