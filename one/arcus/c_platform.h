#pragma once

#if defined(WINDOWS) || defined(PLATFORM_WINDOWS)
    #ifndef ONE_WINDOWS
        #define ONE_WINDOWS
    #endif

    #undef ONE_LINUX
#endif

#if defined(PLATFORM_LINUX) || !defined(ONE_WINDOWS)
    #ifndef ONE_LINUX
        #define ONE_LINUX
    #endif

    #undef ONE_WINDOWS
#endif

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
#endif

#ifdef ONE_LINUX
    #ifndef ONE_EXPORT
        #define ONE_EXPORT __attribute__((visibility("default")))
    #endif

    #include <stdlib.h>
#endif  // Linux.
