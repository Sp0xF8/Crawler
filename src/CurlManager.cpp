#include <CurlManager.hpp>
#include <Logger.hpp>
#include <cstring>


CurlManager::CurlManager() {
    // Initialize a curl session
    LOG("Initializing libcurl");
    curl_global_init(CURL_GLOBAL_DEFAULT);
    this->curl = curl_easy_init();
}

CurlManager::~CurlManager() {
    // Clean up the curl session
    LOG("Cleaning up libcurl");
    curl_easy_cleanup(this->curl);
    // Clean up global libcurl state
    curl_global_cleanup();
}



WebPage* CurlManager::get(const char* url) {
    LOG("Sending GET request to: ", url);

    // Set the URL to request
    curl_easy_setopt(this->curl, CURLOPT_URL, url);

    std::string html_content;

    // Set the callback function to handle the response data
    curl_easy_setopt(this->curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(this->curl, CURLOPT_WRITEDATA, &html_content);

    // Perform the request
    CURLcode res = curl_easy_perform(this->curl);

    // Check if the request was successful
    if (res != CURLE_OK) {
        LOG("Failed to get HTML from: ", url, " - ", curl_easy_strerror(res));
        return nullptr;
    }

    // Create a new WebPage object with the URL and HTML content
    return new WebPage(url, html_content.c_str());
}

size_t CurlManager::write_callback(void* ptr, size_t size, size_t nmemb, void* userdata) {
    size_t realsize = size * nmemb;
    std::string* html_content = static_cast<std::string*>(userdata);

    // Append received data to the string
    html_content->append(static_cast<char*>(ptr), realsize);

    return realsize;
}