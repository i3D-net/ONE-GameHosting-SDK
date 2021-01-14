#pragma once

#if defined(WINDOWS) || defined(PLATFORM_WINDOWS)
    #define ONE_WINDOWS
#else  // If WINDOWS, else LINUX
    #define ONE_LINUX
#endif  // Linux

#if defined(PLATFORM_WINDOWS)
    #define ONE_UNREAL_WINDOWS
    // This disable rapidjson warnings on Windows where __gnuc__
    // is not defined.
    #pragma warning(disable : 4668)
#endif

// Symbol export management.
#ifdef ONE_WINDOWS
    #ifndef ONE_EXPORT
        #define ONE_EXPORT __declspec(dllexport)
    #endif
#else  // If WINDOWS, else LINUX
    #ifndef ONE_EXPORT
        #define ONE_EXPORT __attribute__((visibility("default")))
    #endif

    #include <stdlib.h>
#endif  // Linux.
