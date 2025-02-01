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
    LOG("Sending Getting HTML from: ", url);

    // Set the URL to request
    curl_easy_setopt(this->curl, CURLOPT_URL, url);

    char *html_content = reinterpret_cast<char*>(malloc(1)); // Initial empty string
    html_content[0] = '\0';  // Null-terminate the string


    // Set the callback function to handle the response data
    curl_easy_setopt(this->curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(this->curl, CURLOPT_WRITEDATA, &html_content);

    // Perform the request
    CURLcode res = curl_easy_perform(this->curl);

    // Check if the request was successful
    if(res != CURLE_OK) {
        LOG("Failed to get HTML from: ", url, " - ", curl_easy_strerror(res));
        free(html_content);
        return nullptr;
    }

    // Create a new WebPage object with the URL and HTML content
    WebPage* webpage = new WebPage(url, html_content);

    // Free the allocated memory for the HTML content
    free(html_content);

    return webpage;
}

size_t CurlManager::write_callback(void *ptr, size_t size, size_t nmemb, char **html_content) {
    size_t realsize = size * nmemb;

    // Allocate memory for the new data
    *html_content = reinterpret_cast<char*>(realloc(*html_content, realsize + 1));

    // Append the new data to the existing data
    strncat(*html_content, reinterpret_cast<char*>(ptr), realsize);

    return realsize;
}