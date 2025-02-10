#include <CurlManager.hpp>
#include <Logger.hpp>
#include <cstring>


/**
 * @brief Constructs a new CurlManager object.
 * 
 * This constructor initializes a libcurl session by calling `curl_global_init` 
 * with `CURL_GLOBAL_DEFAULT` and then initializes an easy curl handle using 
 * `curl_easy_init`. It also logs the initialization process.
 */
CurlManager::CurlManager() {
    // Initialize a curl session
    LOG("Initializing libcurl");
    curl_global_init(CURL_GLOBAL_DEFAULT);
    this->curl = curl_easy_init();
}

/**
 * @brief Destructor for the CurlManager class.
 *
 * This destructor is responsible for cleaning up the libcurl session and 
 * the global libcurl state. It ensures that resources allocated by libcurl 
 * are properly released when a CurlManager object is destroyed.
 */
CurlManager::~CurlManager() {
    // Clean up the curl session
    LOG("Cleaning up libcurl");
    curl_easy_cleanup(this->curl);
    // Clean up global libcurl state
    curl_global_cleanup();
}



/**
 * @brief Sends a GET request to the specified URL and retrieves the HTML content.
 *
 * This function uses libcurl to send a GET request to the given URL. It sets the URL,
 * specifies a callback function to handle the response data, and performs the request.
 * If the request is successful, it returns a pointer to a new std::string containing
 * the HTML content. If the request fails, it logs an error message and returns nullptr.
 *
 * @param url The URL to send the GET request to.
 * @return std::string* A pointer to a std::string containing the HTML content if the request is successful, or nullptr if the request fails.
 */
std::string* CurlManager::get(const char* url) {
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
    return new std::string(html_content);
}

size_t CurlManager::write_callback(void* ptr, size_t size, size_t nmemb, void* userdata) {
    size_t realsize = size * nmemb;
    std::string* html_content = static_cast<std::string*>(userdata);

    // Append received data to the string
    html_content->append(static_cast<char*>(ptr), realsize);

    return realsize;
}