#pragma once

#if defined(WINDOWS) || defined(PLATFORM_WINDOWS)
    #define ONE_WINDOWS
#endif

// Symbol export management.
#ifdef ONE_WINDOWS
    #ifndef ONE_EXPORT
        #define ONE_EXPORT __declspec(dllexport)
    #endif
#else // If WINDOWS, else LINUX
    #ifndef ONE_EXPORT
        #define ONE_EXPORT __attribute__((visibility("default")))
    #endif

    #include <stdlib.h>
#endif // Linux.
