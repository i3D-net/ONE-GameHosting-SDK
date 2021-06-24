#pragma once

#include <cstddef>

/** @file c_api.h
    @brief The C API for Ping Sites and Pingers.

    This is the externally facing interface for the ONE Ping SDK, exposing
    management an Ping Sites and Pingers to allow communication with the ONE Platform.

    It is a C API for ABI compatibility and widespread language binding support.

    The basic use pattern to support a ping sites integration is:
    1. Create a ping sites and update it unitl it's status is ready.
    2. Create a pingers and initialize it with the ping sites IP addresses.
    3. Update the pingers to have up-to-date pings.
    3. Properly cleanup the sites_getter and pingers as needed.

    NOTE ON THREAD SAFETY:
    > Working with sites_getter and pingers objects returned from
   i3d_ping_sites_getter_create and i3d_ping_pingers_create is thread safe. > Thread safe
   functions are identified in the function declaration comments.
*/

#include <one/ping/c_platform.h>

#include <one/ping/c_error.h>

#ifdef __cplusplus
extern "C" {
#endif

///
/// Status of a i3D Ping Sites.
typedef enum I3dSitesGetterStatus {
    I3D_SITES_GETTER_STATUS_UNINITIALIZED = 0,
    I3D_SITES_GETTER_STATUS_INITIALIZED = 1,
    I3D_SITES_GETTER_STATUS_WAITING = 2,
    I3D_SITES_GETTER_STATUS_ERROR = 3,
    I3D_SITES_GETTER_STATUS_READY = 4
} I3dSitesGetterStatus;

///
/// Status of a i3D Pingers.
typedef enum I3dPingersStatus {
    I3D_PINGERS_STATUS_UNINITIALIZED = 0,
    I3D_PINGERS_STATUS_INITIALIZED = 1
} I3dPingersStatus;

// See the c_error.h file for error values.

//------------------------------------------------------------------------------
///@name Opaque types.
/// The API uses the pointer handles to represent internal objects.
///@{

/// Opaque type and handle to a i3D PingSites.
struct I3dSitesGetter;
typedef I3dSitesGetter *I3dSitesGetterPtr;

/// Opaque type and handle to a i3D Pingers.
struct I3dPingers;
typedef I3dPingers *I3dPingersPtr;

/// Opaque type and handle to a i3D IpList.
struct I3dIpList;
typedef I3dIpList *I3dIpListPtr;

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
///@name PingSites interface.
/// The PingSites is a main object of the C API. A single sites_getter should be created
/// per gane client.
///@{

/// Logging level that is passed to the optional log callback function.
/// \sa i3d_ping_sites_getter_create
typedef enum I3dPingLogLevel {
    I3D_PING_LOG_LEVEL_INFO = 0,
    I3D_PING_LOG_LEVEL_ERROR
} I3dPingLogLevel;

/// Creates a new Ping Sites. Create, update, shutdown and destroy should be called to
/// complete the life cycle. Thread-safe. A pointer to the I3dSitesGetterPtr is passed in
/// and the pointer will be set to newly created I3dSitesGetterPtr. Use Example:
///     I3dSitesGetterPtr *sites_getter;
///     auto err = i3d_ping_sites_getter_create(&sites_getter);
///     if (i3d_ping_is_error(err)) {
///         myLogger.log(i3d_ping_error_text(err));
///     }
///     // Use the sites_getter, e.g. i3d_ping_sites_getter_update(sites_getter), etc...
///     i3d_ping_sites_getter_destroy(sites_getter);
/// @param sites_getter A null sites_getter pointer, which will be set to a new
/// sites_getter. \sa i3d_ping_sites_getter_destroy \sa i3d_ping_sites_getter_update \sa
/// i3d_ping_sites_getter_status
/// @param http_get_callback A non-null callback that do the http request.
/// @param userdata a nullable pointer that will be provided to the callback.
I3D_PING_EXPORT I3dPingError i3d_ping_sites_getter_create(
    I3dSitesGetterPtr *sites_getter,
    void (*callback)(const char *url,
                     void (*)(bool success, const char *json, void *parsing_userdata),
                     void *parsing_userdata, void *http_get_metadata),
    void *userdata);

/// Log callback function to allow the integration to handle internal i3D Ping Sites
/// logs with its own logger.
/// @param level The severity of the logged information.
/// @param message The actual message.
/// \sa i3d_ping_sites_getter_create
typedef void (*I3dPingLogFn)(void *userdata, I3dPingLogLevel level, const char *message);

/// Sets a custom logger that can handle logs from inside of the sites_getter. By
/// default the sites_getter will log to standard out.
/// @param sites_getter A non-null sites_getter pointer.
/// @param log_cb Optional log callback function. Can be null.
/// @param userdata Optional user data that will be passed back to the callback.
I3D_PING_EXPORT I3dPingError i3d_ping_sites_getter_set_logger(
    I3dSitesGetterPtr sites_getter, I3dPingLogFn log_cb, void *userdata);

/// Destroys a sites_getter instance created via i3d_ping_sites_getter_create. Destroy
/// will shutdown the sites_getter first, if it is active. Note although other
/// sites_getter functions are thread safe, this one is not. A sites_getter must not be
/// destroyed or interacted with on other threads.
/// @param sites_getter A non-null sites_getter pointer.
I3D_PING_EXPORT void i3d_ping_sites_getter_destroy(I3dSitesGetterPtr sites_getter);

/// Update the sites_getter. This must be called frequently until the HTTP Get callback
/// has finished. The sites_getter status will be ready then. If the sites_getter status
/// is error, calling i3d_ping_sites_getter_update again will resend the HTTP Get request.
/// @param sites_getter A non-null sites_getter pointer. Thread-safe.
I3D_PING_EXPORT I3dPingError i3d_ping_sites_getter_update(I3dSitesGetterPtr sites_getter);

/// Obtains the status of the sites_getter. Thread-safe. The passed in pointer is set
/// to the status value.
/// @param sites_getter A non-null sites_getter pointer.
/// @param status A pointer to a status enum value to be set.
I3D_PING_EXPORT I3dPingError i3d_ping_sites_getter_status(
    I3dSitesGetterPtr const sites_getter, I3dSitesGetterStatus *status);

/// Get the site list size.
/// @param sites_getter A non-null sites_getter pointer. Thread-safe.
/// @param size A non-null pointer to set the size on.
I3D_PING_EXPORT I3dPingError
i3d_ping_sites_getter_site_list_size(I3dSitesGetterPtr sites_getter, unsigned int *size);

/// Get the continent id of the site at the given position in the site list.
/// @param sites_getter A non-null sites_getter pointer. Thread-safe.
/// @param pos The position in the list . Must be less than
/// i3d_ping_sites_getter_site_list_size.
/// @param continent_id Non-null pointer to set the id on.
I3D_PING_EXPORT I3dPingError i3d_ping_sites_getter_list_site_continent_id(
    I3dSitesGetterPtr sites_getter, unsigned int pos, int *continent_id);

/// Get the country of the site at the given position in the site list.
/// The maximum character size of a country is 63 chars and with the null terminated
/// string the buffer size must be of 64.
/// @param sites_getter A non-null sites_getter pointer. Thread-safe.
/// @param pos The position in the list . Must be less than
/// i3d_ping_sites_getter_site_list_size.
/// @param country Non-null character array with size 64 to set the country on.
I3D_PING_EXPORT I3dPingError i3d_ping_sites_getter_list_site_country(
    I3dSitesGetterPtr sites_getter, unsigned int pos, char country[64]);

/// Get the data center location id of the site at the given position in the site
/// list.
/// @param sites_getter A non-null sites_getter pointer. Thread-safe.
/// @param pos The position in the list . Must be less than
/// i3d_ping_sites_getter_site_list_size.
/// @param dc_location_id Non-null pointer to set the id on.
I3D_PING_EXPORT I3dPingError i3d_ping_sites_getter_list_site_dc_location_id(
    I3dSitesGetterPtr sites_getter, unsigned int pos, int *dc_location_id);

/// Get the data center location name of the site at the given position in the site
/// list. The maximum character size of an dc_location_name is 63 chars and with the null
/// terminated string the buffer size must be of 64.
/// @param sites_getter A non-null sites_getter pointer. Thread-safe.
/// @param pos The position in the list . Must be less than
/// i3d_ping_sites_getter_site_list_size.
/// @param dc_location_name Non-null character array of size 64 to set the name on.
I3D_PING_EXPORT I3dPingError i3d_ping_sites_getter_list_site_dc_location_name(
    I3dSitesGetterPtr sites_getter, unsigned int pos, char dc_location_name[64]);

/// Get the hostname of the site at the given position in the site list.
/// The maximum character size of an hostname is 63 chars and with the null terminated
/// string the buffer size must be of 64.
/// @param sites_getter A non-null sites_getter pointer. Thread-safe.
/// @param pos The position in the list . Must be less than
/// i3d_ping_sites_getter_site_list_size.
/// @param hostname Non-null character array of size 64 to set the name on.
I3D_PING_EXPORT I3dPingError i3d_ping_sites_getter_list_site_hostname(
    I3dSitesGetterPtr sites_getter, unsigned int pos, char hostname[64]);

/// Get the Ipv4 list size of the site at the given position in the site list.
/// @param sites_getter A non-null sites_getter pointer. Thread-safe.
/// @param pos The position in the list . Must be less than
/// i3d_ping_sites_getter_site_list_size.
/// @param size Non-null pointer to set the size on.
I3D_PING_EXPORT I3dPingError i3d_ping_sites_getter_list_site_ipv4_size(
    I3dSitesGetterPtr sites_getter, unsigned int pos, unsigned int *size);

/// Get the IPv4 at the given position of the site at the given position in the site list.
/// The maximum character size of an IPv6 is 15 chars and with the null terminated string
/// the buffer size must be of 16.
/// @param sites_getter A non-null sites_getter pointer. Thread-safe.
/// @param pos The position in the list . Must be less than
/// i3d_ping_sites_getter_site_list_size.
/// @param ip_pos The position in the ip list . Must be less than
/// i3d_ping_sites_getter_list_site_ip_size.
/// @param ip Non-null character array of size 16 to set the ipv4 on.
I3D_PING_EXPORT I3dPingError i3d_ping_sites_getter_list_site_ipv4_ip(
    I3dSitesGetterPtr sites_getter, unsigned int pos, unsigned int ip_pos, char ip[16]);

/// Get the Ipv6 list size of the site at the given position in the site list.
/// @param sites_getter A non-null sites_getter pointer. Thread-safe.
/// @param pos The position in the list . Must be less than
/// i3d_ping_sites_getter_site_list_size.
/// @param size Non-null pointer to set the size on.
I3D_PING_EXPORT I3dPingError i3d_ping_sites_getter_list_site_ipv6_size(
    I3dSitesGetterPtr sites_getter, unsigned int pos, unsigned int *size);

/// Get the IPv6 at the given position of the site at the given position in the site list.
/// The maximum character size of an IPv6 is 45 chars and with the null terminated string
/// the buffer size must be of 46.
/// @param sites_getter A non-null sites_getter pointer. Thread-safe.
/// @param pos The position in the list . Must be less than
/// i3d_ping_sites_getter_site_list_size.
/// @param ip_pos The position in the ip list . Must be less than
/// i3d_ping_sites_getter_list_site_ip_size.
/// @param ip Non-null character array of size 46 to set the ipv6 on.
I3D_PING_EXPORT I3dPingError i3d_ping_sites_getter_list_site_ipv6_ip(
    I3dSitesGetterPtr sites_getter, unsigned int pos, unsigned int ip_pos, char ip[46]);

/// Get the IPv4 list of all the sites contained in the ping_site.
/// @param sites_getter A non-null sites_getter pointer. Thread-safe.
/// @param ip_list A non-null sites_getter pointer. Thread-safe.
I3D_PING_EXPORT I3dPingError
i3d_ping_sites_getter_ipv4_list(I3dSitesGetterPtr sites_getter, I3dIpListPtr ip_list);

/// Get the IPv6 list of all the sites contained in the ping_site.
/// @param sites_getter A non-null sites_getter pointer. Thread-safe.
/// @param ip_list A non-null sites_getter pointer. Thread-safe.
I3D_PING_EXPORT I3dPingError
i3d_ping_sites_getter_ipv6_list(I3dSitesGetterPtr sites_getter, I3dIpListPtr ip_list);

//------------------------------------------------------------------------------
///@}
///@name Pingers interface.
/// The Pingers is a main object of the C API. A single Pingers should be created
/// per gane client.
///@{

/// Creates a new Pingers. Create, update, shutdown and destroy should be called to
/// complete the life cycle. Thread-safe. A pointer to the I3dPingersPtr is passed in
/// and the pointer will be set to newly created I3dPingersPtr. Use Example:
///     I3dPingersPtr *pingers;
///     auto err = i3d_ping_pingers_create(&pingers);
///     if (i3d_ping_is_error(err)) {
///         myLogger.log(i3d_ping_error_text(err));
///     }
///     // Use the pingers, e.g. i3d_ping_pingers_update(pingers), etc...
///     i3d_ping_pingers_destroy(pingers);
/// @param pingers A null pingers pointer, which will be set to a new pingers.
/// \sa i3d_ping_pingers_destroy
/// \sa i3d_ping_pingers_update
/// \sa i3d_ping_pingers_status
/// @param ip_list Non-null pointer of an IpList.
I3D_PING_EXPORT I3dPingError i3d_ping_pingers_create(I3dPingersPtr *pingers,
                                                     I3dIpListPtr const ip_list);

/// Sets a custom logger that can handle logs from inside of the pingers. By
/// default the pingers will log to standard out.
/// @param pingers A non-null pingers pointer.
/// @param log_cb Optional log callback function. Can be null.
/// @param userdata Optional user data that will be passed back to the callback.
I3D_PING_EXPORT I3dPingError i3d_ping_pingers_set_logger(I3dPingersPtr pingers,
                                                         I3dPingLogFn log_cb,
                                                         void *userdata);

/// Destroys a pingers instance created via pingers. Destroy will
/// shutdown the pingers first, if it is active. Note although other pingers
/// functions are thread safe, this one is not. A pingers must not be destroyed or
/// interacted with on other threads.
/// @param pingers A non-null pingers pointer.
I3D_PING_EXPORT void i3d_ping_pingers_destroy(I3dPingersPtr pingers);

/// Update the pingers. This must be called frequently (e.g. each frame)
/// This will update the ping of all the ips in the ip_list.
/// @param pingers A non-null pingers pointer. Thread-safe.
I3D_PING_EXPORT I3dPingError i3d_ping_pingers_update(I3dPingersPtr pingers);

/// Obtains the status of the pingers. Thread-safe. The passed in pointer is set
/// to the status value.
/// @param pingers A non-null pingers pointer.
/// @param status A pointer to a status enum value to be set.
I3D_PING_EXPORT I3dPingError i3d_ping_pingers_status(I3dPingersPtr const pingers,
                                                     I3dPingersStatus *status);

/// Get the pingers size.
/// @param pingers A non-null pingers pointer. Thread-safe.
/// @param size Non-null pointer to set the size on.
I3D_PING_EXPORT I3dPingError i3d_ping_pingers_size(I3dPingersPtr pingers,
                                                   unsigned int *size);

/// Get the ping statistics (i.e.: last time, average time and ping response counts) of
/// the site at the given position in the site list.
/// @param pingers A non-null pingers pointer. Thread-safe.
/// @param pos The position in the list . Must be less than
/// i3d_ping_pingers_size.
/// @param last_time Non-null pointer to set the last_time on.
/// @param average_time Non-null pointer to set the average_time on.
/// @param min_time Non-null pointer to set the min_time on.
/// @param max_time Non-null pointer to set the max_time on.
/// @param median_time Non-null pointer to set the median_time on.
/// @param ping_response_count Non-null pointer to set the ping response count on.
I3D_PING_EXPORT I3dPingError i3d_ping_pingers_statistics(
    I3dPingersPtr pingers, unsigned int pos, int *last_time, double *average_time,
    int *min_time, int *max_time, double *median_time, unsigned int *ping_response_count);

/// Gets true if at least one site was pinged recently.
/// @param pingers A non-null pingers pointer. Thread-safe.
/// @param result Non-null pointer to set the result on.
I3D_PING_EXPORT I3dPingError
i3d_ping_pingers_at_least_one_site_has_been_pinged(I3dPingersPtr pingers, bool *result);

/// Gets true if all the sites were pinged recently.
/// @param pingers A non-null pingers pointer. Thread-safe.
/// @param result Non-null pointer to set the result on.
I3D_PING_EXPORT I3dPingError
i3d_ping_pingers_all_sites_have_been_pinged(I3dPingersPtr pingers, bool *result);

//------------------------------------------------------------------------------
///@}
///@name I3dIpListPtr interface.
/// The I3dIpListPtr contains the I3dIpListPtr passed between the I3dPingSitePtr and
/// I3dPingersPtr.
///@{

/// Creates a new I3dIpListPtr. Create and destroy should be called to
/// complete the life cycle. Thread-safe. A pointer to the I3dIpListPtr is passed in
/// and the pointer will be set to newly created I3dIpListPtr. Use Example:
///     I3dIpListPtr *ip_list;
///     auto err = i3d_ip_list_create(&pingers);
///     if (i3d_ping_is_error(err)) {
///         myLogger.log(i3d_ping_error_text(err));
///     }
///     // Use the ip_list, e.g. i3d_ip_list_clear(ip_list), etc...
///     i3d_ip_list_destroy(ip_list);
/// @param ip_list A null ip_list pointer, which will be set to a new ip_list.
/// \sa i3d_ip_list_destroy
/// \sa i3d_ip_list_clear
/// \sa i3d_ip_list_push_back
/// \sa i3d_ip_list_size
/// \sa i3d_ip_list_ip
I3D_PING_EXPORT I3dPingError i3d_ping_ip_list_create(I3dIpListPtr *ip_list);

/// Destroys a I3dIpListPtr instance created via i3d_ping_ip_list_create. Note although
/// other pingers functions are thread safe, this one is not. A IpList must not be
/// destroyed or interacted with on other threads.
/// @param ip_list A non-null ip_list pointer.
I3D_PING_EXPORT void i3d_ping_ip_list_destroy(I3dIpListPtr ip_list);

/// Clears the IpList.
/// @param ip_list A non-null ip_list pointer.
I3D_PING_EXPORT I3dPingError i3d_ping_ip_list_clear(I3dIpListPtr ip_list);

/// Push back an IP in the IpList.
/// @param ip_list A non-null pingers pointer.
/// @param ip A non-null string pointer.
I3D_PING_EXPORT I3dPingError i3d_ping_ip_list_push_back(I3dIpListPtr ip_list,
                                                        const char *ip);

/// Size of an IpList.
/// @param ip_list A non-null pingers pointer.
/// @param size Non-null pointer to set the size on.
I3D_PING_EXPORT I3dPingError i3d_ping_ip_list_size(I3dIpListPtr const ip_list,
                                                   unsigned int *size);

/// Size of an IP string in the IpList at the given position.
/// @param ip_list A non-null pingers pointer.
/// @param pos the position in the list.
/// @param size Non-null pointer to set the size on.
I3D_PING_EXPORT I3dPingError i3d_ping_ip_list_ip_size(I3dIpListPtr const ip_list,
                                                      unsigned int pos,
                                                      unsigned int *size);

/// Gets the ip at the given position in the IpList.
/// @param ip_list A non-null pingers pointer.
/// @param pos the position in the list.
/// @param ip Non-null pointer to set the ipv4 on.
/// @param size Size of the value buffer that can be written to. Must be at least as big
/// as i3d_ip_list_ip_size.
I3D_PING_EXPORT I3dPingError i3d_ping_ip_list_ip(I3dIpListPtr const ip_list,
                                                 unsigned int pos, char *ip,
                                                 unsigned int size);

//------------------------------------------------------------------------------
///@}

#ifdef __cplusplus
};
#endif
