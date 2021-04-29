#pragma once

#include <cstddef>

/** @file c_api.h
    @brief The C API for Ping Client.

    This is the externally facing interface for the ONE Ping SDK, exposing
    management an Ping Client to allow communication with the ONE Platform.

    It is a C API for ABI compatibility and widespread language binding support.

    The basic use pattern to support a ping client integration is:
    1. Create a client and update it.
    2. Get the server ping latency from the client.
    3. Properly cleanup the client as needed.

    NOTE ON THREAD SAFETY:
    > Working with client objects returned from i3d_ping_client_create is thread safe.
    > Thread safe functions are identified in the function declaration comments.
*/

#include <one/ping/c_platform.h>

#include <one/ping/c_error.h>

#ifdef __cplusplus
extern "C" {
#endif

///
/// Status of a i3D Ping Client.
typedef enum I3dPingClientStatus {
    I3D_PING_STATUS_WAITING_FOR_BACKEND = 0,
    I3D_PING_STATUS_READY,
    I3D_PING_STATUS_ERROR
} I3dPingClientStatus;

// See the c_error.h file for error values.

//------------------------------------------------------------------------------
///@name Opaque types.
/// The API uses the pointer handles to represent internal objects.
///@{

/// Opaque type and handle to a i3D Ping Client .
struct I3dPingClient;
typedef I3dPingClient *I3dPingClientPtr;

//------------------------------------------------------------------------------
///@}
///@name Environment.
///@{

/// Optional custom memory alloc override.
/// If set, must be set at init time, before using any other APIs. If this is called,
/// then i3d_ping_allocator_set_free must also be called. If this is called, then
/// i3d_ping_allocator_set_free and i3d_ping_allocator_set_realloc must also be called.
/// @param callback A function that takes a size in bytes of memory to allocate.
/// @sa i3d_ping_allocator_set_free
/// @sa i3d_ping_allocator_set_realloc
I3D_PING_EXPORT void i3d_ping_allocator_set_alloc(void *(*callback)(unsigned int size));

/// Optional custom memory free override.
/// If set, must be set at init time, before using any other APIs.
/// @param callback An existing pointer p to free.
I3D_PING_EXPORT void i3d_ping_allocator_set_free(void (*callback)(void *));

/// Optional custom memory realloc override.
/// If set, must be set at init time, before using any other APIs.
/// @param callback A function taking an existing pointer and a new size. See
/// the standard c realloc requirements for behavior.
I3D_PING_EXPORT void i3d_ping_allocator_set_realloc(void *(*callback)(void *,
                                                                      unsigned int size));

//------------------------------------------------------------------------------
///@}
///@name Client interface.
/// The Client is the main object of the C API. A single client should be created
/// per "ping client".
///@{

/// Logging level that is passed to the optional log callback function.
/// \sa i3d_ping_client_create
typedef enum I3dPingLogLevel {
    I3D_PING_LOG_LEVEL_INFO = 0,
    I3D_PING_LOG_LEVEL_ERROR
} I3dPingLogLevel;

/// Creates a new Ping Client. Create, update, shutdown and destroy should be called to
/// complete the life cycle. Thread-safe. A pointer to the I3dPingClientPtr is passed in
/// and the pointer will be set to newly created I3dPingClientPtr. Use Example:
///     I3dPingClientPtr client;
///     auto err = i3d_ping_client_create(&client);
///     if (i3d_ping_is_error(err)) {
///         myLogger.log(i3d_ping_error_text(err));
///     }
///     // Use the client, e.g. i3d_ping_client_update(client), etc...
///     i3d_ping_client_destroy(client);
/// @param client A null client pointer, which will be set to a new client.
/// \sa i3d_ping_client_destroy
/// \sa i3d_ping_client_update
/// \sa i3d_ping_client_status
I3D_PING_EXPORT I3dPingError i3d_ping_client_create(I3dPingClientPtr *client);

/// Log callback function to allow the integration to handle internal i3D Ping Client
/// logs with its own logger.
/// @param level The severity of the logged information.
/// @param message The actual message.
/// \sa i3d_ping_client_create
typedef void (*I3dPingLogFn)(void *userdata, I3dPingLogLevel level, const char *message);

/// Sets a custom logger that can handle logs from inside of the client. By
/// default the client will log to standard out.
/// @param client A non-null client pointer.
/// @param log_cb Optional log callback function. Can be null.
/// @param userdata Optional user data that will be passed back to the callback.
I3D_PING_EXPORT I3dPingError i3d_ping_client_set_logger(I3dPingClientPtr client,
                                                        I3dPingLogFn log_cb,
                                                        void *userdata);

/// Destroys a client instance created via i3d_ping_client_create. Destroy will
/// shutdown the client first, if it is active. Note although other client functions
/// are thread safe, this one is not. A client must not be destroyed or interacted
/// with on other threads.
/// @param client A non-null client pointer.
I3D_PING_EXPORT void i3d_ping_client_destroy(I3dPingClientPtr client);

/// Update the client. This must be called frequently (e.g. each frame)
/// TODO wording
/// @param client A non-null client pointer. Thread-safe.
I3D_PING_EXPORT I3dPingError i3d_ping_client_update(I3dPingClientPtr client);

/// Obtains the status of the client. Thread-safe. The passed in pointer is set
/// to the status value.
/// @param client A non-null client pointer.
/// @param status A pointer to a status enum value to be set.
I3D_PING_EXPORT I3dPingError i3d_ping_client_status(I3dPingClientPtr const client,
                                                    I3dPingClientStatus *status);

/// Get the server list size.
/// @param client A non-null client pointer. Thread-safe.
I3D_PING_EXPORT I3dPingError i3d_ping_client_server_list_size(I3dPingClientPtr client,
                                                              unsigned int *size);

/// Get the ip of the server at the given position in the server list.
/// @param client A non-null client pointer. Thread-safe.
/// @param pos The position in the list . Must be less than
/// i3d_ping_client_list_server_size.
/// @param ip Non-null pointer to set the ip on.
/// @param size Size of the value buffer that can be written to.
I3D_PING_EXPORT I3dPingError i3d_ping_client_list_server_ip(I3dPingClientPtr client,
                                                            unsigned int pos,
                                                            const char *ip,
                                                            unsigned int size);

/// Get the latest ping latency of the server at the given position in the server list.
/// @param client A non-null client pointer. Thread-safe.
/// @param pos The position in the list . Must be less than
/// i3d_ping_client_list_server_size.
/// @param latest Ping latency of the server in milliseconds. A negative value means that
/// the server was never successfuly pinged.
I3D_PING_EXPORT I3dPingError i3d_ping_client_list_server_latest_ping_latency(
    I3dPingClientPtr client, unsigned int pos, int *latest);

/// Get the latest ping latency of the server at the given position in the server list.
/// @param client A non-null client pointer. Thread-safe.
/// @param pos The position in the list . Must be less than
/// i3d_ping_client_list_server_size.
/// @param mean Ping latency of the server in milliseconds. A negative value means that
/// the server was never successfuly pinged.
I3D_PING_EXPORT I3dPingError i3d_ping_client_list_server_mean_ping_latency(
    I3dPingClientPtr client, unsigned int pos, int *mean);

#ifdef __cplusplus
};
#endif
