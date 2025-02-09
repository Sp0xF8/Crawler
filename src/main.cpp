#include <stdio.h>
#include <curl/curl.h>

#include <WebPage.hpp>


int main() {


    WebPage* webpage = new WebPage("https://en.wikipedia.org/wiki/James_Joyce");

    if(webpage != nullptr) {
        webpage->scrape();
        webpage->write_markdown();
        delete webpage;
    }


    return 0;
}
