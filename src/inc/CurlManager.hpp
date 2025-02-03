#pragma once

#include <curl/curl.h>

#include <WebPage.hpp>

class CurlManager {

    public:

        CurlManager();
        ~CurlManager();

        WebPage* get(const char* url);

    private:

        CURL *curl;

        static size_t write_callback(void* ptr, size_t size, size_t nmemb, void* userdata);
};

extern CurlManager curl_manager;
