#pragma once

#include <cstddef>

/** @file c_api.h
    @brief The C API for Arcus Server.

    This is the externally facing interface for the One Game Hosting SDK, exposing
    management an Arcus Server to allow communication with the One Platform.

    It is a C API for ABI compatibility and widespread language binding support.

    The basic use pattern to support a game server integration is:
    1. Create a server. Tell it to listen on a port and update it.
    2. Use the outgoing and incoming message functions like
       one_server_send_live_state_response and one_server_set_allocated_callback
       to send and receive the supported Arcus Messages, which includes providing
       the high level game state to the Arcus platform.
    3. If needed, use the Message API functions like one_message_val_int to read
    key/value pair details from the more complex messages.
    4. Properly cleanup the server as needed.
*/

#define ONE_EXPORT
#define ONE_STDCALL

//#include <cstddef>
#include <one/arcus/c_error.h>

#ifdef __cplusplus
extern "C" {
#endif

// See the c_error.h file for error values.

// Server status values.
typedef enum OneServerStatus {
    ONE_SERVER_STATUS_UNINITIALIZED = 0,
    ONE_SERVER_STATUS_INITIALIZED,
    ONE_SERVER_STATUS_WAITING_FOR_CLIENT,
    ONE_SERVER_STATUS_HANDSHAKE,
    ONE_SERVER_STATUS_READY,
    ONE_SERVER_STATUS_ERROR
} OneServerStatus;

//------------------------------------------------------------------------------
// Opaque types.

// A One Arcus Server, that can be hosted and communicate with an incoming Arcus
// connection.
struct OneServer;
typedef OneServer *OneServerPtr;

// A message corresponding to the messages documented in the Arcus protocol API:
// https://www.i3d.net/docs/one/odp/Game-Integration/Management-Protocol/Arcus-V2/request-response/
struct OneMessage;
typedef OneMessage *OneMessagePtr;

// Optional Array value that may be present within a OneMessage.
struct OneArray;
typedef OneArray *OneArrayPtr;

// Optional Object value that may be present within a OneMessage.
struct OneObject;
typedef OneObject *OneObjectPtr;

//------------------------------------------------------------------------------
///@name Environment.
///@{

/// Provide custom memory alloc.
/// Must be set at init time, before using any other APIs. If this is called,
// then one_allocator_set_free must also be called.
void one_allocator_set_alloc(void *(callback)(unsigned int size));

/// Provide custom memory free.
/// Must be set at init time, before using any other APIs.
void one_allocator_set_free(void(callback)(void *));

//------------------------------------------------------------------------------
///@}
///@name Server interface.
/// The Server is the main object of the C API. One server should be created
/// per "game server". The server needs to be updated often to send and receive
/// messages from an Arcus Client.
///@{

/// Creates a new Arcus Server. Each Game Server must have one corresponding
/// Arcus Server. Listen, update, shutdown and destroy should be called to complete the
/// life cycle.
/// @param server A null server pointer, which will be set to a new server.
/// \sa one_server_destroy
/// \sa one_server_listen
/// \sa one_server_update
/// \sa one_server_status
OneError one_server_create(OneServerPtr *server);

/// Destroys a server instance created via one_server_create. Destroy will
/// shutdown the server first, if it is active.
/// @param server A non-null server pointer.
void one_server_destroy(OneServerPtr server);

/// Closes the listen connection, if any and resets the server to creation state.
/// @param server A non-null server pointer.
OneError one_server_shutdown(OneServerPtr server);

/// Start listening on the given port. This should be called before
/// update. Listening will fail and return an error if the port is already in
/// use.
/// @param server A non-null server pointer.
OneError one_server_listen(OneServerPtr server, unsigned int port);

/// Update the server. This must be called frequently (e.g. each frame) to
/// process incoming and outgoing communications. Incoming messages trigger
/// their respective incoming callbacks during the call to update. If the a
/// callback for a message is not set then the message is ignored.
/// @param server A non-null server pointer.
OneError one_server_update(OneServerPtr server);

/// Returns the status of the server
/// @param server A non-null server pointer.
/// @param status A pointer to a status enum value to be set.
OneError one_server_status(OneServerPtr const server, OneServerStatus *status);

//------------------------------------------------------------------------------
///@}
///@name Message main interface
/// One Servers and Clients communicate over TCP by sending Messages. A Message
/// contains an identifying code id, and can have variable number of Key/Value
/// pairs set on them.
///@{

/// Creates a new outgoing message. one_message_destroy must be called to free
/// the message.
/// @param message A non-null pointer to a Message pointer to set the new
/// message on.
OneError one_message_create(OneMessagePtr *message);

/// Must be called whenever finished with a message.
/// @param message A non-null Message pointer.
void one_message_destroy(OneMessagePtr message);

/// Reset can be used to re-use message objects, removing any values set on it.
/// @param message A non-null Message pointer.
OneError one_message_reset(OneMessagePtr message);

/// Retrieves the Message's Arcus Opcode value, identifying the Message type.
/// @param message A non-null Message pointer.
/// @param code A non-null pointer to a code to set.
OneError one_message_code(OneMessagePtr message, int *code);

/// Set the opcode value of the message. Not needed in integration code, which
/// should be using the predefined messages which set the appropriate opcode for
/// the message.
/// @param message A non-null Message pointer.
/// @param code A valid arcus message opcode.
OneError one_message_set_code(OneMessagePtr message, int code);

//------------------------------------------------------------------------------
///@}
///@name Message key/value getters
///@{

/// Queries if the given message key contains a value type.
/// @param message A non-null message.
/// @param key A non-null key string.
/// @param result A non-null pointer to a bool to set the result on.
OneError one_message_is_val_bool(OneMessagePtr message, const char *key, bool *result);
OneError one_message_is_val_int(OneMessagePtr message, const char *key, bool *result);
OneError one_message_is_val_string(OneMessagePtr message, const char *key, bool *result);
OneError one_message_is_val_array(OneMessagePtr message, const char *key, bool *result);
OneError one_message_is_val_object(OneMessagePtr message, const char *key, bool *result);

//------------------------------------------------------------------------------
///@}
///@name Message key/value getters
///@{

/// Sets the given key value pair on the message.
/// @param message A non-null message.
/// @param key A non-null key string.
/// @param val A non-null pointer to a value to get for the given key.
OneError one_message_val_bool(OneMessagePtr message, const char *key, bool *val);
OneError one_message_val_int(OneMessagePtr message, const char *key, int *val);
/// Returns the number of characters in the string. This does not include a trailing null
/// character.
/// @return May return of ONE_ERROR_MESSAGE_*.
/// @param message A non-null message.
/// @param key The key of the value to return.
/// @param size A non-null int pointer to set the size on.
OneError one_message_val_string_size(OneMessagePtr message, const char *key, int *size);
/// Writes the key value to the given character buffer.
/// @return May return of ONE_ERROR_OBJECT_*.
/// @param message A non-null message.
/// @param key The key of the value to return.
/// @param size Size of the value buffer that can be written to. Must be equal
/// to size obtained via one_message_val_string_size.
OneError one_message_val_string(OneMessagePtr message, const char *key, char *val,
                                int size);
OneError one_message_val_array(OneMessagePtr message, const char *key, OneArrayPtr val);
OneError one_message_val_object(OneMessagePtr message, const char *key, OneObjectPtr val);
OneError one_message_val_root_object(OneMessagePtr message, OneObjectPtr val);

//------------------------------------------------------------------------------
///@}
///@name Message key/value setters
///@{

/// Sets the given key value pair on the message.
/// @param message A non-null message.
/// @param key A non-null key string.
/// @param val The value to set for the given key.
OneError one_message_set_val_bool(OneMessagePtr message, const char *key, bool val);
OneError one_message_set_val_int(OneMessagePtr message, const char *key, int val);
OneError one_message_set_val_string(OneMessagePtr message, const char *key,
                                    const char *val);
OneError one_message_set_val_array(OneMessagePtr message, const char *key,
                                   OneArrayPtr val);
OneError one_message_set_val_object(OneMessagePtr message, const char *key,
                                    OneObjectPtr val);
OneError one_message_set_val_root_object(OneMessagePtr message, OneObjectPtr val);

//------------------------------------------------------------------------------
///@}
///@name Array main interface
///@{

/// Creates a new array. Must be freed with one_array_destroy.
/// @param array A null pointer to a OneArrayPtr, to be set with new array.
OneError one_array_create(OneArrayPtr *array);

/// Destroys array.
/// @param array A non-null OneArrayPtr, to be deleted.
void one_array_destroy(OneArrayPtr array);

/// Makes a copy of the array. The destination must have been created via
/// one_array_create.
OneError one_array_copy(OneArrayPtr source, OneArrayPtr destination);

/// Clears the array to an empty initialized state.
/// @param array A non-null OneArrayPtr.
OneError one_array_clear(OneArrayPtr array);

/// Reserves array space.
/// @param array A non-null OneArrayPtr  .
/// @param size Number of total elements the array should contain.
OneError one_array_reserve(OneArrayPtr array, int size);

/// Sets the given value to true if the array is empty.
/// @param array A non-null OneArrayPtr.
/// @param empty A non-null bool pointer to set the result on.
OneError one_array_is_empty(OneArrayPtr array, bool *empty);

/// Returns the number of elements pushed to the array.
/// @param array A non-null OneArrayPtr.
/// @param size A non-null int pointer to set the result on.
OneError one_array_size(OneArrayPtr array, int *size);

/// Returns the total size, allocated via one_array_reserve, of the array.
/// @param array A non-null OneArrayPtr.
/// @param capacity A non-null int pointer to set the result on.
OneError one_array_capacity(OneArrayPtr array, int *capacity);

//------------------------------------------------------------------------------
///@}
///@name Array element addition and removal
///@{

/// Adds an element value of the given type to the back of the array. The array
/// must have sufficient free space, that is the capacity must be greater than
/// the size.
/// @param array A pointer that will be set to point to the new OneArrayPtr.
/// @param val The value to add as as new element.
OneError one_array_push_back_bool(OneArrayPtr array, bool val);
OneError one_array_push_back_int(OneArrayPtr array, int val);
OneError one_array_push_back_string(OneArrayPtr array, const char *val);
OneError one_array_push_back_array(OneArrayPtr array, OneArrayPtr val);
OneError one_array_push_back_object(OneArrayPtr array, OneObjectPtr val);

/// Removes last element of the array, if any.
/// @param array A pointer that will be set to point to the new OneArrayPtr.
OneError one_array_pop_back(OneArrayPtr array);

//------------------------------------------------------------------------------
///@}
///@name Array element value type checkers
///@{

/// Checks whether the given key is of the given type.
/// @param array A pointer that will be set to point to the new OneArrayPtr.
/// @param pos The element index to inspect.
/// @param result A non-null bool pointer to set the result to.
/// @return May return ONE_ERROR_ARRAY_* or ONE_ERROR_NONE.
OneError one_array_is_val_bool(OneArrayPtr array, unsigned int pos, bool *result);
OneError one_array_is_val_int(OneArrayPtr array, unsigned int pos, bool *result);
OneError one_array_is_val_string(OneArrayPtr array, unsigned int pos, bool *result);
OneError one_array_is_val_array(OneArrayPtr array, unsigned int pos, bool *result);
OneError one_array_is_val_object(OneArrayPtr array, unsigned int pos, bool *result);

//------------------------------------------------------------------------------
///@}
///@name Array Getters
///@{

/// Retrieves and sets the value of the given type from the array. The given value
/// pointer must be non-null and will have the return value set on it.
/// @return May return of ONE_ERROR_ARRAY_*.
/// @param array A valid array created via one_array_create.
/// @param key The key of the value to return.
OneError one_array_val_bool(OneArrayPtr array, unsigned int pos, bool *val);
OneError one_array_val_int(OneArrayPtr array, unsigned int pos, int *val);
/// Returns the number of characters in the string. This does not include a trailing null
/// character.
/// @return May return of ONE_ERROR_ARRAY_*.
/// @param array A valid array created via one_array_create.
/// @param key The key of the value to return.
/// @param size A non-null int pointer to set the size on.
OneError one_array_val_string_size(OneArrayPtr array, unsigned int pos, int *size);
/// Writes the key value to the given character buffer.
/// @return May return of ONE_ERROR_ARRAY_*.
/// @param array A valid array created via one_array_create.
/// @param key The key of the value to return.
/// @param size Size of the value buffer that can be written to. Must be equal
/// to size obtained via one_array_val_string_size.
OneError one_array_val_string(OneArrayPtr array, unsigned int pos, char *val,
                              int val_size);
OneError one_array_val_array(OneArrayPtr array, unsigned int pos, OneArrayPtr val);
OneError one_array_val_object(OneArrayPtr array, unsigned int pos, OneObjectPtr val);

//------------------------------------------------------------------------------
///@}
///@name Array Setters
///@{

/// Allows setting a sub key/value pair on the array of the given type.
/// @param array A valid object created via one_array_create.
OneError one_array_set_val_bool(OneArrayPtr array, unsigned int pos, bool val);
OneError one_array_set_val_int(OneArrayPtr array, unsigned int pos, int val);
OneError one_array_set_val_string(OneArrayPtr array, unsigned int pos, const char *val);
OneError one_array_set_val_array(OneArrayPtr array, unsigned int pos, OneArrayPtr val);
OneError one_array_set_val_object(OneArrayPtr array, unsigned int pos, OneObjectPtr val);

//------------------------------------------------------------------------------
///@}
///@name Object main interface
///@{

/// Create a new object that can be used as a key value in a OneMessage.
/// one_object_destroy must be called to free the object.
/// @param object A pointer that will be set to point to the new OneObjectPtr.
OneError one_object_create(OneObjectPtr *object);

/// Must be called to free an object created by one_object_create.
/// @param object A non-null object pointer created via one_object_create.
void one_object_destroy(OneObjectPtr object);

OneError one_object_copy(OneObjectPtr source, OneObjectPtr destination);
OneError one_object_clear(OneObjectPtr object);
OneError one_object_is_empty(OneObjectPtr object, bool *empty);
OneError one_object_remove_key(OneObjectPtr object, const char *key);

//------------------------------------------------------------------------------
///@}
///@name Object Key Value type checkers
///@{

/// Checks whether the given key is of the given type.
/// @param object A pointer that will be set to point to the new OneObjectPtr.
/// @param key The key to lookup.
/// @param result A non-null bool pointer to set the result to.
/// @return May return ONE_ERROR_OBJECT_* or ONE_ERROR_NONE.
OneError one_object_is_val_bool(OneObjectPtr object, const char *key, bool *result);
OneError one_object_is_val_int(OneObjectPtr object, const char *key, bool *result);
OneError one_object_is_val_string(OneObjectPtr object, const char *key, bool *result);
OneError one_object_is_val_array(OneObjectPtr object, const char *key, bool *result);
OneError one_object_is_val_object(OneObjectPtr object, const char *key, bool *result);

//------------------------------------------------------------------------------
///@}
///@name Object Getters
///@{

/// Retrieves and sets the value of the given type from the object. The given value
/// pointer must be non-null and will have the return value set on it.
/// @return May return of ONE_ERROR_OBJECT_*.
/// @param object A valid object created via one_object_create.
/// @param key The key of the value to return.
OneError one_object_val_bool(OneObjectPtr object, const char *key, bool *val);
OneError one_object_val_int(OneObjectPtr object, const char *key, int *val);
/// Returns the number of characters in the string. This does not include a trailing null
/// character.
/// @return May return of ONE_ERROR_OBJECT_*.
/// @param object A valid object created via one_object_create.
/// @param key The key of the value to return.
/// @param size A non-null int pointer to set the size on.
OneError one_object_val_string_size(OneObjectPtr object, const char *key, int *size);
/// Writes the key value to the given character buffer.
/// @return May return of ONE_ERROR_OBJECT_*.
/// @param object A valid object created via one_object_create.
/// @param key The key of the value to return.
/// @param size Size of the value buffer that can be written to. Must be equal
/// to size obtained via one_object_val_string_size.
OneError one_object_val_string(OneObjectPtr object, const char *key, char *val, int size);
OneError one_object_val_array(OneObjectPtr object, const char *key, OneArrayPtr val);
OneError one_object_val_object(OneObjectPtr object, const char *key, OneObjectPtr val);

//------------------------------------------------------------------------------
///@}
///@name Object Setters
///@{

/// Allows setting a sub key/value pair on the object of the given type.
/// @param object A valid object created via one_object_create.
OneError one_object_set_val_bool(OneObjectPtr object, const char *key, bool val);
OneError one_object_set_val_int(OneObjectPtr object, const char *key, int val);
OneError one_object_set_val_string(OneObjectPtr object, const char *key, const char *val);
OneError one_object_set_val_array(OneObjectPtr object, const char *key, OneArrayPtr val);
OneError one_object_set_val_object(OneObjectPtr object, const char *key,
                                   OneObjectPtr val);

//------------------------------------------------------------------------------
///@}
///@name Arcus Message Outgoing Message senders.
/// See the [One Arcus protocol documentation
/// website](https://www.i3d.net/docs/one/odp/Game-Integration/Management-Protocol/Arcus-V2/request-response/)
/// for more information.
///
/// Message preperation functions add the required fields to the predefined
/// messages. The caller may add additional fields before sending, if the
/// One Platform is configured to utilize those extra fields.
///@{

OneError one_message_prepare_live_state_response(int players, int max_players,
                                                 const char *name, const char *map,
                                                 const char *mode, const char *version,
                                                 OneMessagePtr message);
OneError one_message_prepare_application_instance_set_status_request(
    int status, OneMessagePtr message);

/// Send the Arcus API server live_state_response opcode message.
/// Message Mandatory Content:
/// {
///   "players" : 0,
///   "maxPlayers" : 0,
///   "name" : "",
///   "map" : "",
///   "mode" : "",
///   "version" : "",
/// }
OneError one_server_send_live_state_response(OneServerPtr server, OneMessagePtr message);

/// send application_instance_set_status_request.
/// Message Empty Content:
/// {
///   "status": 0
/// }
OneError one_server_send_application_instance_set_status_request(OneServerPtr server,
                                                                 OneMessagePtr message);

//------------------------------------------------------------------------------
///@}
///@name Arcus Message Incoming Message handling.
/// See the [One Arcus protocol documentation
/// website](https://www.i3d.net/docs/one/odp/Game-Integration/Management-Protocol/Arcus-V2/request-response/)
/// for more information.
///@{

/// Registers a callback to be called when a soft stop message is received. The
/// process should stop at its earliest convenience. If the server process is
/// still active after the given timeout (seconds), then One will terminate the
/// process directly.
/// @param server Non-null server pointer.
/// @param callback Callback to be called during a call to one_server_update, if
///                 the message is received from the Client.
/// @param data Optional user data that will be passed back to the callback.
OneError one_server_set_soft_stop_callback(OneServerPtr server,
                                           void (*callback)(void *data, int timeout),
                                           void *data);

/// Register the callback to be called when an allocated message is received.
/// The game server must read the given array data and be ready to accept
/// players in the directed gameplay environment (e.g. map, mode), if specified
/// and required by the game's One platform configuration.
/// @param server Non-null server pointer.
/// @param callback Callback to be called during a call to one_server_update, if
///                 the message is received from the Client.
/// @param data Optional user data that will be passed back to the callback.
OneError one_server_set_allocated_callback(OneServerPtr server,
                                           void (*callback)(void *data, void *array),
                                           void *data);

/// Register the callback to be notified of a metadata.
/// The `void *array` will be of type OneArrayPtr or the callback will error out.
/// @param server Non-null server pointer.
/// @param callback Callback to be called during a call to one_server_update, if
///                 the message is received from the Client.
/// @param data Optional user data that will be passed back to the callback.
OneError one_server_set_meta_data_callback(OneServerPtr server,
                                           void (*callback)(void *data, void *array),
                                           void *data);

/// Register the callback to be notified of a host_information_response.
/// The `void *object` will be of type OneObjectPtr or the callback will error out.
/// @param server Non-null server pointer.
/// @param callback Callback to be called during a call to one_server_update, if
///                 the message is received from the Client.
/// @param data Optional user data that will be passed back to the callback.
OneError one_server_set_host_information_response_callback(
    OneServerPtr server, void (*callback)(void *data, void *object), void *data);

/// Register the callback to be notified of a application_instance_information_response.
/// The `void *object` will be of type OneObjectPtr or the callback will error out.
/// @param server Non-null server pointer.
/// @param callback Callback to be called during a call to one_server_update, if
///                 the message is received from the Client.
/// @param data Optional user data that will be passed back to the callback.
OneError one_server_set_application_instance_information_response_callback(
    OneServerPtr server, void (*callback)(void *data, void *object), void *data);

///@}

#ifdef __cplusplus
};
#endif
