#include <catch.hpp>

#include <curl/curl.h>

#include <string>
#include <memory>

TEST_CASE("libcurl https GET", "[http]") {
    const std::string url = "https://en.wikipedia.org/wiki/Main_Page";

    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);

    curl = curl_easy_init();
    REQUIRE(curl);

    // Avoid SSL certificate issues.
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    // Avoid SSL certificate issues.
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    // To avoid spam in the logs.
    curl_easy_setopt(curl, CURLOPT_NOBODY, 1);
    // Don't wait forever, time out after 10 seconds.
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);
    // Follow HTTP redirects if necessary.
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    // Set HTTP url.
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    res = curl_easy_perform(curl);

    REQUIRE(res == CURLE_OK);

    curl_easy_cleanup(curl);
    curl_global_cleanup();
}

namespace {
std::size_t callback(const char *in, std::size_t size, std::size_t num,
                     std::string *out) {
    const std::size_t total_Bytes = size * num;
    out->append(in, total_Bytes);
    return total_Bytes;
}
}  // namespace

TEST_CASE("libcurl https GET with json response", "[http]") {
    const std::string url = "http://date.jsontest.com/";

    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);

    curl = curl_easy_init();
    REQUIRE(curl);

    // Avoid SSL certificate issues.
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    // Avoid SSL certificate issues.
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    // Don't wait forever, time out after 10 seconds.
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);
    // Follow HTTP redirects if necessary.
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    // Set HTTP url.
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

    // Hook up data handling function.
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);

    std::unique_ptr<std::string> http_data(new std::string());
    // Hook up data container (will be passed as the last parameter to the
    // callback handling function).  Can be any pointer type, since it will
    // internally be passed as a void pointer.
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, http_data.get());

    res = curl_easy_perform(curl);
    REQUIRE(res == CURLE_OK);

    long http_code(0);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

    // To avoid out of quota error.
    if (http_code == 503) {
        return;
    }
    REQUIRE(http_code == 200);
    REQUIRE(!http_data.get()->empty());

    curl_easy_cleanup(curl);
    curl_global_cleanup();
}
