#include <catch.hpp>

#include <curl/curl.h>

TEST_CASE("libcurl https", "[ping]") {
    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);

    curl = curl_easy_init();
    REQUIRE(curl);

    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

    // To avoid spam in the logs.
    curl_easy_setopt(curl, CURLOPT_NOBODY, 1);

    curl_easy_setopt(curl, CURLOPT_URL, "https://en.wikipedia.org/wiki/Main_Page");
    res = curl_easy_perform(curl);

    REQUIRE(res == CURLE_OK);

    curl_easy_cleanup(curl);
    curl_global_cleanup();
}
