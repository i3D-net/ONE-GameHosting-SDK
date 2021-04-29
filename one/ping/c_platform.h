#pragma once

// Symbol export management.
#if defined(PLATFORM_LINUX) && PLATFORM_LINUX
    #ifndef I3D_PING_LINUX
        #define I3D_PING_LINUX
    #endif

    #ifndef I3D_PING_EXPORT
        #define I3D_PING_EXPORT __attribute__((visibility("default")))
    #endif

    #include <stdlib.h>
#elif defined(PLATFORM_WINDOWS) && PLATFORM_WINDOWS
    #define I3D_PING_UNREAL_WINDOWS
    // This disable rapidjson warnings on Windows where __gnuc__
    // is not defined.
    #pragma warning(disable : 4668)

    #ifndef I3D_PING_WINDOWS
        #define I3D_PING_WINDOWS
    #endif

    #ifndef I3D_PING_EXPORT
        #define I3D_PING_EXPORT __declspec(dllexport)
    #endif
#elif defined(WINDOWS)
    #ifndef I3D_PING_WINDOWS
        #define I3D_PING_WINDOWS
    #endif

    #ifndef I3D_PING_EXPORT
        #define I3D_PING_EXPORT __declspec(dllexport)
    #endif
#else  // If not windows, Linux.
    #ifndef I3D_PING_LINUX
        #define I3D_PING_LINUX
    #endif

    #ifndef I3D_PING_EXPORT
        #define I3D_PING_EXPORT __attribute__((visibility("default")))
    #endif

    #include <stdlib.h>
#endif  // Linux.
