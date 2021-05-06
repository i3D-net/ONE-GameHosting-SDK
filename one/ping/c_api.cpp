#include <one/ping/c_api.h>

#include <one/ping/allocator.h>
#include <one/ping/c_platform.h>
#include <one/ping/error.h>
#include <one/ping/ip_list.h>
#include <one/ping/logger.h>
#include <one/ping/pingers.h>
#include <one/ping/sites_getter.h>
#include <one/ping/internal/site_information.h>
#include <one/ping/types.h>

namespace i3d {
namespace ping {
namespace {

void allocator_set_alloc(void *(*callback)(unsigned int size)) {
    // Wrapper for size_t.
    auto wrapper = [callback](size_t size) -> void * {
        return callback(static_cast<unsigned int>(size));
    };
    allocator::set_alloc(wrapper);
}

void allocator_set_free(void(callback)(void *)) {
    allocator::set_free(callback);
}

void allocator_set_realloc(void *(*callback)(void *, unsigned int size)) {
    // Wrapper for size_t.
    auto wrapper = [callback](void *p, size_t size) -> void * {
        return callback(p, static_cast<unsigned int>(size));
    };
    allocator::set_realloc(wrapper);
}

I3dPingError sites_getter_create(I3dSitesGetterPtr *sites_getter) {
    if (sites_getter == nullptr) {
        return I3D_PING_ERROR_VALIDATION_SITES_GETTER_IS_NULLPTR;
    }

    auto p = allocator::create<SitesGetter>();
    if (p == nullptr) {
        return I3D_PING_ERROR_SITES_GETTER_ALLOCATION_FAILED;
    }

    *sites_getter = (I3dSitesGetterPtr)p;
    return I3D_PING_ERROR_NONE;
}

I3dPingError sites_getter_set_logger(I3dSitesGetterPtr sites_getter, I3dPingLogFn log_cb,
                                     void *userdata) {
    if (sites_getter == nullptr) {
        return I3D_PING_ERROR_VALIDATION_SITES_GETTER_IS_NULLPTR;
    }

    auto wrapper = [log_cb](void *userdata, LogLevel level, const String &message) {
        log_cb(userdata, static_cast<I3dPingLogLevel>(level), message.c_str());
    };
    Logger logger(wrapper, userdata);

    auto p = (SitesGetter *)(sites_getter);
    p->set_logger(logger);
    return I3D_PING_ERROR_NONE;
}

void sites_getter_destroy(I3dSitesGetterPtr sites_getter) {
    if (sites_getter == nullptr) {
        return;
    }

    auto p = (SitesGetter *)(sites_getter);
    allocator::destroy<SitesGetter>(p);
}

I3dPingError sites_getter_init(I3dSitesGetterPtr sites_getter) {
    auto p = (SitesGetter *)(sites_getter);
    if (p == nullptr) {
        return I3D_PING_ERROR_VALIDATION_SITES_GETTER_IS_NULLPTR;
    }

    return p->init();
}

I3dPingError sites_getter_update(I3dSitesGetterPtr sites_getter) {
    auto p = (SitesGetter *)(sites_getter);
    if (p == nullptr) {
        return I3D_PING_ERROR_VALIDATION_SITES_GETTER_IS_NULLPTR;
    }

    return p->update();
}

I3dPingError sites_getter_status(I3dSitesGetterPtr const sites_getter,
                                 I3dSitesGetterStatus *status) {
    auto p = (SitesGetter *)(sites_getter);
    if (p == nullptr) {
        return I3D_PING_ERROR_VALIDATION_SITES_GETTER_IS_NULLPTR;
    }

    if (status == nullptr) {
        return I3D_PING_ERROR_VALIDATION_STATUS_IS_NULLPTR;
    }

    *status = static_cast<I3dSitesGetterStatus>(p->status());
    return I3D_PING_ERROR_NONE;
}

I3dPingError sites_getter_set_http_get_callback(
    I3dSitesGetterPtr sites_getter,
    void (*callback)(const char *url,
                     void (*)(bool success, const char *json, void *parsing_userdata),
                     void *parsing_userdata, void *http_get_metadata),
    void *userdata) {
    auto p = (SitesGetter *)(sites_getter);
    if (p == nullptr) {
        return I3D_PING_ERROR_VALIDATION_SITES_GETTER_IS_NULLPTR;
    }

    if (callback == nullptr) {
        return I3D_PING_ERROR_VALIDATION_HTTP_GET_CALLBACK_IS_NULLPTR;
    }

    return p->set_http_get_callback(callback, userdata);
}

I3dPingError sites_getter_site_list_size(I3dSitesGetterPtr sites_getter,
                                         unsigned int *size) {
    auto p = (SitesGetter *)(sites_getter);
    if (p == nullptr) {
        return I3D_PING_ERROR_VALIDATION_SITES_GETTER_IS_NULLPTR;
    }

    if (size == nullptr) {
        return I3D_PING_ERROR_VALIDATION_SIZE_IS_NULLPTR;
    }

    *size = p->sites_size();
    return I3D_PING_ERROR_NONE;
}

I3dPingError sites_getter_list_site_continent_id(I3dSitesGetterPtr sites_getter,
                                                 unsigned int pos, int *continent_id) {
    auto p = (SitesGetter *)(sites_getter);
    if (p == nullptr) {
        return I3D_PING_ERROR_VALIDATION_SITES_GETTER_IS_NULLPTR;
    }

    if (continent_id == nullptr) {
        return I3D_PING_ERROR_VALIDATION_CONTINENT_ID_IS_NULLPTR;
    }

    return p->site_continent_id(pos, *continent_id);
}

I3dPingError sites_getter_list_site_country_size(I3dSitesGetterPtr sites_getter,
                                                 unsigned int pos, unsigned int *size) {
    auto p = (SitesGetter *)(sites_getter);
    if (p == nullptr) {
        return I3D_PING_ERROR_VALIDATION_SITES_GETTER_IS_NULLPTR;
    }

    if (size == nullptr) {
        return I3D_PING_ERROR_VALIDATION_SIZE_IS_NULLPTR;
    }

    String s;
    auto err = p->site_country(pos, s);
    if (i3d_ping_is_error(err)) {
        return err;
    }

    *size = s.size();
    return I3D_PING_ERROR_NONE;
}

I3dPingError sites_getter_list_site_country(I3dSitesGetterPtr sites_getter,
                                            unsigned int pos, char *country,
                                            unsigned int size) {
    auto p = (SitesGetter *)(sites_getter);
    if (p == nullptr) {
        return I3D_PING_ERROR_VALIDATION_SITES_GETTER_IS_NULLPTR;
    }

    if (country == nullptr) {
        return I3D_PING_ERROR_VALIDATION_COUNTRY_IS_NULLPTR;
    }

    String s;
    auto err = p->site_country(pos, s);
    if (i3d_ping_is_error(err)) {
        return err;
    }

    if (size < static_cast<unsigned int>(s.size())) {
        return I3D_PING_ERROR_VALIDATION_SIZE_IS_TOO_SMALL;
    }

    for (size_t i = 0; i < s.size(); ++i) {
        country[i] = s[i];
    }

    return I3D_PING_ERROR_NONE;
}

I3dPingError sites_getter_list_site_dc_location_id(I3dSitesGetterPtr sites_getter,
                                                   unsigned int pos,
                                                   int *dc_location_id) {
    auto p = (SitesGetter *)(sites_getter);
    if (p == nullptr) {
        return I3D_PING_ERROR_VALIDATION_SITES_GETTER_IS_NULLPTR;
    }

    if (dc_location_id == nullptr) {
        return I3D_PING_ERROR_VALIDATION_DC_LOCATION_ID_IS_NULLPTR;
    }

    return p->site_dc_location_id(pos, *dc_location_id);
}

I3dPingError sites_getter_list_site_dc_location_name_size(I3dSitesGetterPtr sites_getter,
                                                          unsigned int pos,
                                                          unsigned int *size) {
    auto p = (SitesGetter *)(sites_getter);
    if (p == nullptr) {
        return I3D_PING_ERROR_VALIDATION_SITES_GETTER_IS_NULLPTR;
    }

    if (size == nullptr) {
        return I3D_PING_ERROR_VALIDATION_SIZE_IS_NULLPTR;
    }

    String s;
    auto err = p->site_dc_location_name(pos, s);
    if (i3d_ping_is_error(err)) {
        return err;
    }

    *size = s.size();
    return I3D_PING_ERROR_NONE;
}

I3dPingError sites_getter_list_site_dc_location_name(I3dSitesGetterPtr sites_getter,
                                                     unsigned int pos,
                                                     char *dc_location_name,
                                                     unsigned int size) {
    auto p = (SitesGetter *)(sites_getter);
    if (p == nullptr) {
        return I3D_PING_ERROR_VALIDATION_SITES_GETTER_IS_NULLPTR;
    }

    if (dc_location_name == nullptr) {
        return I3D_PING_ERROR_VALIDATION_DC_LOCATION_NAME_IS_NULLPTR;
    }

    String s;
    auto err = p->site_dc_location_name(pos, s);
    if (i3d_ping_is_error(err)) {
        return err;
    }

    if (size < static_cast<unsigned int>(s.size())) {
        return I3D_PING_ERROR_VALIDATION_SIZE_IS_TOO_SMALL;
    }

    for (size_t i = 0; i < s.size(); ++i) {
        dc_location_name[i] = s[i];
    }

    return I3D_PING_ERROR_NONE;
}

I3dPingError sites_getter_list_site_hostname_size(I3dSitesGetterPtr sites_getter,
                                                  unsigned int pos, unsigned int *size) {
    auto p = (SitesGetter *)(sites_getter);
    if (p == nullptr) {
        return I3D_PING_ERROR_VALIDATION_SITES_GETTER_IS_NULLPTR;
    }

    if (size == nullptr) {
        return I3D_PING_ERROR_VALIDATION_SIZE_IS_NULLPTR;
    }

    String s;
    auto err = p->site_hostname(pos, s);
    if (i3d_ping_is_error(err)) {
        return err;
    }

    *size = s.size();
    return I3D_PING_ERROR_NONE;
}

I3dPingError sites_getter_list_site_hostname(I3dSitesGetterPtr sites_getter,
                                             unsigned int pos, char *hostname,
                                             unsigned int size) {
    auto p = (SitesGetter *)(sites_getter);
    if (p == nullptr) {
        return I3D_PING_ERROR_VALIDATION_SITES_GETTER_IS_NULLPTR;
    }

    if (hostname == nullptr) {
        return I3D_PING_ERROR_VALIDATION_HOSTNAME_IS_NULLPTR;
    }

    String s;
    auto err = p->site_hostname(pos, s);
    if (i3d_ping_is_error(err)) {
        return err;
    }

    if (size < static_cast<unsigned int>(s.size())) {
        return I3D_PING_ERROR_VALIDATION_SIZE_IS_TOO_SMALL;
    }

    for (size_t i = 0; i < s.size(); ++i) {
        hostname[i] = s[i];
    }

    return I3D_PING_ERROR_NONE;
}

I3dPingError sites_getter_list_site_ipv4_size(I3dSitesGetterPtr sites_getter,
                                              unsigned int pos, unsigned int *size) {
    auto p = (SitesGetter *)(sites_getter);
    if (p == nullptr) {
        return I3D_PING_ERROR_VALIDATION_SITES_GETTER_IS_NULLPTR;
    }

    if (size == nullptr) {
        return I3D_PING_ERROR_VALIDATION_SIZE_IS_NULLPTR;
    }

    size_t s = 0;
    auto err = p->site_ipv4_size(pos, s);
    if (i3d_ping_is_error(err)) {
        return err;
    }

    *size = static_cast<unsigned int>(s);
    return I3D_PING_ERROR_NONE;
}

I3dPingError sites_getter_list_site_ipv4_ip_size(I3dSitesGetterPtr sites_getter,
                                                 unsigned int pos, unsigned int ip_pos,
                                                 unsigned int *size) {
    auto p = (SitesGetter *)(sites_getter);
    if (p == nullptr) {
        return I3D_PING_ERROR_VALIDATION_SITES_GETTER_IS_NULLPTR;
    }

    if (size == nullptr) {
        return I3D_PING_ERROR_VALIDATION_SIZE_IS_NULLPTR;
    }

    String s;
    auto err = p->site_ipv4(pos, ip_pos, s);
    if (i3d_ping_is_error(err)) {
        return err;
    }

    *size = s.size();
    return I3D_PING_ERROR_NONE;
}

I3dPingError sites_getter_list_site_ipv4_ip(I3dSitesGetterPtr sites_getter,
                                            unsigned int pos, unsigned int ip_pos,
                                            char *ip, unsigned int size) {
    auto p = (SitesGetter *)(sites_getter);
    if (p == nullptr) {
        return I3D_PING_ERROR_VALIDATION_SITES_GETTER_IS_NULLPTR;
    }

    if (ip == nullptr) {
        return I3D_PING_ERROR_VALIDATION_IP_IS_NULLPTR;
    }

    String s;
    auto err = p->site_ipv4(pos, ip_pos, s);
    if (i3d_ping_is_error(err)) {
        return err;
    }

    if (size < static_cast<unsigned int>(s.size())) {
        return I3D_PING_ERROR_VALIDATION_SIZE_IS_TOO_SMALL;
    }

    for (size_t i = 0; i < s.size(); ++i) {
        ip[i] = s[i];
    }

    return I3D_PING_ERROR_NONE;
}

I3dPingError sites_getter_list_site_ipv6_size(I3dSitesGetterPtr sites_getter,
                                              unsigned int pos, unsigned int *size) {
    auto p = (SitesGetter *)(sites_getter);
    if (p == nullptr) {
        return I3D_PING_ERROR_VALIDATION_SITES_GETTER_IS_NULLPTR;
    }

    if (size == nullptr) {
        return I3D_PING_ERROR_VALIDATION_SIZE_IS_NULLPTR;
    }

    size_t s = 0;
    auto err = p->site_ipv6_size(pos, s);
    if (i3d_ping_is_error(err)) {
        return err;
    }
    *size = static_cast<unsigned int>(s);

    return I3D_PING_ERROR_NONE;
}

I3dPingError sites_getter_list_site_ipv6_ip_size(I3dSitesGetterPtr sites_getter,
                                                 unsigned int pos, unsigned int ip_pos,
                                                 unsigned int *size) {
    auto p = (SitesGetter *)(sites_getter);
    if (p == nullptr) {
        return I3D_PING_ERROR_VALIDATION_SITES_GETTER_IS_NULLPTR;
    }

    if (size == nullptr) {
        return I3D_PING_ERROR_VALIDATION_SIZE_IS_NULLPTR;
    }

    String s;
    auto err = p->site_ipv6(pos, ip_pos, s);
    if (i3d_ping_is_error(err)) {
        return err;
    }

    *size = s.size();
    return I3D_PING_ERROR_NONE;
}

I3dPingError sites_getter_list_site_ipv6_ip(I3dSitesGetterPtr sites_getter,
                                            unsigned int pos, unsigned int ip_pos,
                                            char *ip, unsigned int size) {
    auto p = (SitesGetter *)(sites_getter);
    if (p == nullptr) {
        return I3D_PING_ERROR_VALIDATION_SITES_GETTER_IS_NULLPTR;
    }

    if (ip == nullptr) {
        return I3D_PING_ERROR_VALIDATION_IP_IS_NULLPTR;
    }

    String s;
    auto err = p->site_ipv6(pos, ip_pos, s);
    if (i3d_ping_is_error(err)) {
        return err;
    }

    if (size < static_cast<unsigned int>(s.size())) {
        return I3D_PING_ERROR_VALIDATION_SIZE_IS_TOO_SMALL;
    }

    for (size_t i = 0; i < s.size(); ++i) {
        ip[i] = s[i];
    }

    return I3D_PING_ERROR_NONE;
}

I3dPingError sites_getter_ipv4_list(I3dSitesGetterPtr sites_getter,
                                    I3dIpListPtr ip_list) {
    auto p = (SitesGetter *)(sites_getter);
    if (p == nullptr) {
        return I3D_PING_ERROR_VALIDATION_SITES_GETTER_IS_NULLPTR;
    }

    auto ips = (IpList *)(ip_list);
    if (ips == nullptr) {
        return I3D_PING_ERROR_VALIDATION_IP_LIST_IS_NULLPTR;
    }

    return p->ipv4_list(*ips);
}

I3dPingError sites_getter_ipv6_list(I3dSitesGetterPtr sites_getter,
                                    I3dIpListPtr ip_list) {
    auto p = (SitesGetter *)(sites_getter);
    if (p == nullptr) {
        return I3D_PING_ERROR_VALIDATION_SITES_GETTER_IS_NULLPTR;
    }

    auto ips = (IpList *)(ip_list);
    if (ips == nullptr) {
        return I3D_PING_ERROR_VALIDATION_IP_LIST_IS_NULLPTR;
    }

    return p->ipv6_list(*ips);
}

I3dPingError pingers_create(I3dPingersPtr *pingers) {
    if (pingers == nullptr) {
        return I3D_PING_ERROR_VALIDATION_PINGERS_IS_NULLPTR;
    }

    auto p = allocator::create<Pingers>();
    if (p == nullptr) {
        return I3D_PING_ERROR_SITES_GETTER_ALLOCATION_FAILED;
    }

    *pingers = (I3dPingersPtr)p;
    return I3D_PING_ERROR_NONE;
}

void pingers_destroy(I3dPingersPtr pingers) {
    if (pingers == nullptr) {
        return;
    }

    auto p = (Pingers *)(pingers);
    allocator::destroy<Pingers>(p);
}

I3dPingError pingers_init(I3dPingersPtr pingers, I3dIpListPtr const ip_list) {
    auto p = (Pingers *)(pingers);
    if (p == nullptr) {
        return I3D_PING_ERROR_VALIDATION_PINGERS_IS_NULLPTR;
    }

    auto ips = (IpList *)(ip_list);
    if (ips == nullptr) {
        return I3D_PING_ERROR_VALIDATION_IP_LIST_IS_NULLPTR;
    }

    return p->init(*ips);
}

I3dPingError pingers_update(I3dPingersPtr pingers) {
    auto p = (Pingers *)(pingers);
    if (p == nullptr) {
        return I3D_PING_ERROR_VALIDATION_PINGERS_IS_NULLPTR;
    }

    return p->update();
}

I3dPingError pingers_status(I3dPingersPtr const pingers, I3dPingersStatus *status) {
    auto p = (Pingers *)(pingers);
    if (p == nullptr) {
        return I3D_PING_ERROR_VALIDATION_PINGERS_IS_NULLPTR;
    }

    if (status == nullptr) {
        return I3D_PING_ERROR_VALIDATION_STATUS_IS_NULLPTR;
    }

    *status = static_cast<I3dPingersStatus>(p->status());
    return I3D_PING_ERROR_NONE;
}

I3dPingError pingers_size(I3dPingersPtr pingers, unsigned int *size) {
    auto p = (Pingers *)(pingers);
    if (p == nullptr) {
        return I3D_PING_ERROR_VALIDATION_PINGERS_IS_NULLPTR;
    }

    if (size == nullptr) {
        return I3D_PING_ERROR_VALIDATION_SIZE_IS_NULLPTR;
    }

    *size = p->size();
    return I3D_PING_ERROR_NONE;
}

I3dPingError pingers_last_time(I3dPingersPtr pingers, unsigned int pos,
                               int *duration_ms) {
    auto p = (Pingers *)(pingers);
    if (p == nullptr) {
        return I3D_PING_ERROR_VALIDATION_PINGERS_IS_NULLPTR;
    }

    if (duration_ms == nullptr) {
        return I3D_PING_ERROR_VALIDATION_LATEST_TIME_IS_NULLPTR;
    }

    auto err = p->last_time(pos, *duration_ms);
    if (i3d_ping_is_error(err)) {
        return err;
    }

    return I3D_PING_ERROR_NONE;
}

I3dPingError pingers_average_time(I3dPingersPtr pingers, unsigned int pos,
                                  double *duration_ms) {
    auto p = (Pingers *)(pingers);
    if (p == nullptr) {
        return I3D_PING_ERROR_VALIDATION_PINGERS_IS_NULLPTR;
    }

    if (duration_ms == nullptr) {
        return I3D_PING_ERROR_VALIDATION_AVERAGE_TIME_NULLPTR;
    }

    auto err = p->average_time(pos, *duration_ms);
    if (i3d_ping_is_error(err)) {
        return err;
    }

    return I3D_PING_ERROR_NONE;
}

I3dPingError pingers_ping_response_count(I3dPingersPtr pingers, unsigned int pos,
                                         unsigned int *ping_response_count) {
    auto p = (Pingers *)(pingers);
    if (p == nullptr) {
        return I3D_PING_ERROR_VALIDATION_PINGERS_IS_NULLPTR;
    }

    if (ping_response_count == nullptr) {
        return I3D_PING_ERROR_VALIDATION_PING_COUNT_IS_NULLPTR;
    }

    auto err = p->ping_response_count(pos, *ping_response_count);
    if (i3d_ping_is_error(err)) {
        return err;
    }

    return I3D_PING_ERROR_NONE;
}

I3dPingError at_least_one_site_has_been_pinged(I3dPingersPtr pingers, bool *result) {
    auto p = (Pingers *)(pingers);
    if (p == nullptr) {
        return I3D_PING_ERROR_VALIDATION_PINGERS_IS_NULLPTR;
    }

    if (result == nullptr) {
        return I3D_PING_ERROR_VALIDATION_RESULT_IS_NULLPTR;
    }

    auto err = p->at_least_one_site_has_been_pinged(*result);
    if (i3d_ping_is_error(err)) {
        return err;
    }

    return I3D_PING_ERROR_NONE;
}

I3dPingError all_sites_have_been_pinged(I3dPingersPtr pingers, bool *result) {
    auto p = (Pingers *)(pingers);
    if (p == nullptr) {
        return I3D_PING_ERROR_VALIDATION_PINGERS_IS_NULLPTR;
    }

    if (result == nullptr) {
        return I3D_PING_ERROR_VALIDATION_RESULT_IS_NULLPTR;
    }

    auto err = p->all_sites_have_been_pinged(*result);
    if (i3d_ping_is_error(err)) {
        return err;
    }

    return I3D_PING_ERROR_NONE;
}

I3dPingError ip_list_create(I3dIpListPtr *ip_list) {
    if (ip_list == nullptr) {
        return I3D_PING_ERROR_VALIDATION_IP_LIST_IS_NULLPTR;
    }

    auto ips = allocator::create<IpList>();
    if (ips == nullptr) {
        return I3D_PING_ERROR_IP_LIST_ALLOCATION_FAILED;
    }

    *ip_list = (I3dIpListPtr)ips;
    return I3D_PING_ERROR_NONE;
}

void ip_list_destroy(I3dIpListPtr ip_list) {
    if (ip_list == nullptr) {
        return;
    }

    auto ips = (IpList *)(ip_list);
    allocator::destroy<IpList>(ips);
}

I3dPingError ip_list_clear(I3dIpListPtr ip_list) {
    auto ips = (IpList *)(ip_list);
    if (ips == nullptr) {
        return I3D_PING_ERROR_VALIDATION_IP_LIST_IS_NULLPTR;
    }

    ips->clear();
    return I3D_PING_ERROR_NONE;
}

I3dPingError ip_list_push_back(I3dIpListPtr ip_list, const char *ip) {
    auto ips = (IpList *)(ip_list);
    if (ips == nullptr) {
        return I3D_PING_ERROR_VALIDATION_IP_LIST_IS_NULLPTR;
    }

    if (ip == nullptr) {
        return I3D_PING_ERROR_VALIDATION_IP_IS_NULLPTR;
    }

    ips->push_back(String(ip));
    return I3D_PING_ERROR_NONE;
}

I3dPingError ip_list_size(I3dIpListPtr const ip_list, unsigned int *size) {
    auto ips = (IpList *)(ip_list);
    if (ips == nullptr) {
        return I3D_PING_ERROR_VALIDATION_IP_LIST_IS_NULLPTR;
    }

    if (size == nullptr) {
        return I3D_PING_ERROR_VALIDATION_SIZE_IS_NULLPTR;
    }

    size_t s = 0;
    ips->size(s);
    *size = static_cast<unsigned int>(s);
    return I3D_PING_ERROR_NONE;
}

I3dPingError ip_list_ip_size(I3dIpListPtr const ip_list, unsigned int pos,
                             unsigned int *size) {
    auto ips = (IpList *)(ip_list);
    if (ips == nullptr) {
        return I3D_PING_ERROR_VALIDATION_IP_LIST_IS_NULLPTR;
    }

    if (size == nullptr) {
        return I3D_PING_ERROR_VALIDATION_SIZE_IS_NULLPTR;
    }

    String ip;
    auto err = ips->ip(pos, ip);
    if (i3d_ping_is_error(err)) {
        return err;
    }

    *size = static_cast<unsigned int>(ip.size());
    return I3D_PING_ERROR_NONE;
}

I3dPingError ip_list_ip(I3dIpListPtr const ip_list, unsigned int pos, char *ip,
                        unsigned int *size) {
    auto ips = (IpList *)(ip_list);
    if (ips == nullptr) {
        return I3D_PING_ERROR_VALIDATION_PINGERS_IS_NULLPTR;
    }

    if (size == nullptr) {
        return I3D_PING_ERROR_VALIDATION_SIZE_IS_NULLPTR;
    }

    String s;
    auto err = ips->ip(pos, s);
    if (i3d_ping_is_error(err)) {
        return err;
    }

    if (*size < static_cast<unsigned int>(s.size())) {
        return I3D_PING_ERROR_VALIDATION_SIZE_IS_TOO_SMALL;
    }

    for (size_t i = 0; i < s.size(); ++i) {
        ip[i] = s[i];
    }

    return I3D_PING_ERROR_NONE;
}

}  // namespace
}  // namespace ping
}  // namespace i3d

namespace ping = i3d::ping;

extern "C" {
void i3d_ping_allocator_set_alloc(void *(*callback)(unsigned int size)) {
    ping::allocator_set_alloc(callback);
}

void i3d_ping_allocator_set_free(void (*callback)(void *)) {
    ping::allocator_set_free(callback);
}

void i3d_ping_allocator_set_realloc(void *(*callback)(void *, unsigned int size)) {
    ping::allocator_set_realloc(callback);
}

I3dPingError i3d_ping_sites_getter_create(I3dSitesGetterPtr *sites_getter) {
    return ping::sites_getter_create(sites_getter);
}

I3dPingError i3d_ping_sites_getter_set_logger(I3dSitesGetterPtr sites_getter,
                                              I3dPingLogFn log_cb, void *userdata) {
    return ping::sites_getter_set_logger(sites_getter, log_cb, userdata);
}

void i3d_ping_sites_getter_destroy(I3dSitesGetterPtr sites_getter) {
    return ping::sites_getter_destroy(sites_getter);
}

I3dPingError i3d_ping_sites_getter_init(I3dSitesGetterPtr sites_getter) {
    return ping::sites_getter_init(sites_getter);
}

I3dPingError i3d_ping_sites_getter_update(I3dSitesGetterPtr sites_getter) {
    return ping::sites_getter_update(sites_getter);
}

I3dPingError i3d_ping_sites_getter_status(I3dSitesGetterPtr const sites_getter,
                                          I3dSitesGetterStatus *status) {
    return ping::sites_getter_status(sites_getter, status);
}

I3dPingError i3d_ping_sites_getter_set_http_get_callback(
    I3dSitesGetterPtr sites_getter,
    void (*http_get_callback)(const char *, void (*)(bool, const char *, void *), void *,
                              void *),
    void *userdata) {
    return ping::sites_getter_set_http_get_callback(sites_getter, http_get_callback,
                                                    userdata);
}

I3dPingError i3d_ping_sites_getter_site_list_size(I3dSitesGetterPtr sites_getter,
                                                  unsigned int *size) {
    return ping::sites_getter_site_list_size(sites_getter, size);
}

I3dPingError i3d_ping_sites_getter_list_site_continent_id(I3dSitesGetterPtr sites_getter,
                                                          unsigned int pos,
                                                          int *continent_id) {
    return ping::sites_getter_list_site_continent_id(sites_getter, pos, continent_id);
}

I3dPingError i3d_ping_sites_getter_list_site_country_size(I3dSitesGetterPtr sites_getter,
                                                          unsigned int pos,
                                                          unsigned int *size) {
    return ping::sites_getter_list_site_country_size(sites_getter, pos, size);
}

I3dPingError i3d_ping_sites_getter_list_site_country(I3dSitesGetterPtr sites_getter,
                                                     unsigned int pos, char *country,
                                                     unsigned int size) {
    return ping::sites_getter_list_site_country(sites_getter, pos, country, size);
}

I3dPingError i3d_ping_sites_getter_list_site_dc_location_id(
    I3dSitesGetterPtr sites_getter, unsigned int pos, int *dc_location_id) {
    return ping::sites_getter_list_site_dc_location_id(sites_getter, pos, dc_location_id);
}

I3dPingError i3d_ping_sites_getter_list_site_dc_location_name_size(
    I3dSitesGetterPtr sites_getter, unsigned int pos, unsigned int *size) {
    return ping::sites_getter_list_site_dc_location_name_size(sites_getter, pos, size);
}

I3dPingError i3d_ping_sites_getter_list_site_dc_location_name(
    I3dSitesGetterPtr sites_getter, unsigned int pos, char *dc_location_name,
    unsigned int size) {
    return ping::sites_getter_list_site_dc_location_name(sites_getter, pos,
                                                         dc_location_name, size);
}

I3dPingError i3d_ping_sites_getter_list_site_hostname_size(I3dSitesGetterPtr sites_getter,
                                                           unsigned int pos,
                                                           unsigned int *size) {
    return ping::sites_getter_list_site_hostname_size(sites_getter, pos, size);
}

I3dPingError i3d_ping_sites_getter_list_site_hostname(I3dSitesGetterPtr sites_getter,
                                                      unsigned int pos, char *hostname,
                                                      unsigned int size) {
    return ping::sites_getter_list_site_hostname(sites_getter, pos, hostname, size);
}

I3dPingError i3d_ping_sites_getter_list_site_ipv4_size(I3dSitesGetterPtr sites_getter,
                                                       unsigned int pos,
                                                       unsigned int *size) {
    return ping::sites_getter_list_site_ipv4_size(sites_getter, pos, size);
}

I3dPingError i3d_ping_sites_getter_list_site_ipv4_ip_size(I3dSitesGetterPtr sites_getter,
                                                          unsigned int pos,
                                                          unsigned int ip_pos,
                                                          unsigned int *size) {
    return ping::sites_getter_list_site_ipv4_ip_size(sites_getter, pos, ip_pos, size);
}

I3dPingError i3d_ping_sites_getter_list_site_ipv4_ip(I3dSitesGetterPtr sites_getter,
                                                     unsigned int pos,
                                                     unsigned int ip_pos, char *ip,
                                                     unsigned int size) {
    return ping::sites_getter_list_site_ipv4_ip(sites_getter, pos, ip_pos, ip, size);
}

I3dPingError i3d_ping_sites_getter_list_site_ipv6_size(I3dSitesGetterPtr sites_getter,
                                                       unsigned int pos,
                                                       unsigned int *size) {
    return ping::sites_getter_list_site_ipv6_size(sites_getter, pos, size);
}

I3dPingError i3d_ping_sites_getter_list_site_ipv6_ip_size(I3dSitesGetterPtr sites_getter,
                                                          unsigned int pos,
                                                          unsigned int ip_pos,
                                                          unsigned int *size) {
    return ping::sites_getter_list_site_ipv6_ip_size(sites_getter, pos, ip_pos, size);
}

I3dPingError i3d_ping_sites_getter_list_site_ipv6_ip(I3dSitesGetterPtr sites_getter,
                                                     unsigned int pos,
                                                     unsigned int ip_pos, char *ipv6,
                                                     unsigned int size) {
    return ping::sites_getter_list_site_ipv6_ip(sites_getter, pos, ip_pos, ipv6, size);
}

I3dPingError i3d_ping_sites_getter_ipv4_list(I3dSitesGetterPtr sites_getter,
                                             I3dIpListPtr ip_list) {
    return ping::sites_getter_ipv4_list(sites_getter, ip_list);
}

I3dPingError i3d_ping_sites_getter_list_site_ipv6_list(I3dSitesGetterPtr sites_getter,
                                                       I3dIpListPtr ip_list) {
    return ping::sites_getter_ipv6_list(sites_getter, ip_list);
}

I3dPingError i3d_ping_pingers_create(I3dPingersPtr *pingers) {
    return ping::pingers_create(pingers);
}

void i3d_ping_pingers_destroy(I3dPingersPtr pingers) {
    return ping::pingers_destroy(pingers);
}

I3dPingError i3d_ping_pingers_init(I3dPingersPtr pingers, I3dIpListPtr const ip_list) {
    return ping::pingers_init(pingers, ip_list);
}

I3dPingError i3d_ping_pingers_update(I3dPingersPtr pingers) {
    return ping::pingers_update(pingers);
}

I3dPingError i3d_ping_pingers_status(I3dPingersPtr const pingers,
                                     I3dPingersStatus *status) {
    return ping::pingers_status(pingers, status);
}

I3dPingError i3d_ping_pingers_size(I3dPingersPtr pingers, unsigned int *size) {
    return ping::pingers_size(pingers, size);
}

I3dPingError i3d_ping_pingers_last_time(I3dPingersPtr pingers, unsigned int pos,
                                        int *duration_ms) {
    return ping::pingers_last_time(pingers, pos, duration_ms);
}

I3dPingError i3d_ping_pingers_average_time(I3dPingersPtr pingers, unsigned int pos,
                                           double *duration_ms) {
    return ping::pingers_average_time(pingers, pos, duration_ms);
}

I3dPingError i3d_ping_pingers_ping_response_count(I3dPingersPtr pingers, unsigned int pos,
                                                  unsigned int *ping_response_count) {
    return ping::pingers_ping_response_count(pingers, pos, ping_response_count);
}

I3dPingError i3d_ping_pingers_at_least_one_site_has_been_pinged(I3dPingersPtr pingers,
                                                                bool *result) {
    return ping::at_least_one_site_has_been_pinged(pingers, result);
}

I3dPingError i3d_ping_pingers_all_sites_have_been_pinged(I3dPingersPtr pingers,
                                                         bool *result) {
    return ping::all_sites_have_been_pinged(pingers, result);
}

I3dPingError i3d_ping_ip_list_create(I3dIpListPtr *ip_list) {
    return ping::ip_list_create(ip_list);
}

void i3d_ping_ip_list_destroy(I3dIpListPtr ip_list) {
    return ping::ip_list_destroy(ip_list);
}

I3dPingError i3d_ping_ip_list_clear(I3dIpListPtr ip_list) {
    return ping::ip_list_clear(ip_list);
}

I3dPingError i3d_ping_ip_list_push_back(I3dIpListPtr ip_list, const char *ip) {
    return ping::ip_list_push_back(ip_list, ip);
}

I3dPingError i3d_ping_ip_list_size(I3dIpListPtr const ip_list, unsigned int *size) {
    return ping::ip_list_size(ip_list, size);
}

I3dPingError i3d_ping_ip_list_ip_size(I3dIpListPtr const ip_list, unsigned int pos,
                                      unsigned int *size) {
    return ping::ip_list_ip_size(ip_list, pos, size);
}

I3dPingError i3d_ping_ip_list_ip(I3dIpListPtr const ip_list, unsigned int pos, char *ip,
                                 unsigned int *size) {
    return ping::ip_list_ip(ip_list, pos, ip, size);
}
}
