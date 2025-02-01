#include <stdio.h>
#include <curl/curl.h>

#include <CurlManager.hpp>



int main() {


    WebPage* webpage = curl_manager.get("https://www.example.com");

    if(webpage != nullptr) {
        webpage->scrape();
        delete webpage;
    }


    return 0;
}
