#pragma once

//------------------------------------------------------------------------------
// This is the externally facing interface for the One Game Host SDK.
//
// It is a C API for ABI compatibility and widespread language binding support.
//
// Users call the one_game_host_api function to obtain access and make calls
// against its members.

#define ONE_EXPORT
#define ONE_STDCALL

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------
// Errors.

//------------------------------------------------------------------------------
// Main interface.

struct OneGameHost;
typedef OneGameHost* OneGameHostPtr;

struct OneGameHostApi {
    OneGameHostPtr (*create_host)(void);
    void (*destroy_host)(OneGameHostPtr);
};
typedef OneGameHostApi* OneGameHostApiPtr;

// Main entry point call to get a reference to the API.
ONE_EXPORT OneGameHostApiPtr ONE_STDCALL one_game_host_api();

#ifdef __cplusplus
};
#endif
