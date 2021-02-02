#pragma once

#if defined(PLATFORM_WINDOWS)
    #define ONE_UNREAL_WINDOWS
    // This disable rapidjson warnings on Windows where __gnuc__
    // is not defined.
    #pragma warning(disable : 4668)
#endif

// Symbol export management.
#if defined(PLATFORM_WINDOWS) || defined(WINDOWS)
    #ifndef ONE_WINDOWS
        #define ONE_WINDOWS
    #endif

    #ifndef ONE_EXPORT
        #define ONE_EXPORT __declspec(dllexport)
    #endif
#else // If not windows, Linux.
    #ifndef ONE_LINUX
        #define ONE_LINUX
    #endif

    #ifndef ONE_EXPORT
        #define ONE_EXPORT __attribute__((visibility("default")))
    #endif

    #include <stdlib.h>
#endif  // Linux.
