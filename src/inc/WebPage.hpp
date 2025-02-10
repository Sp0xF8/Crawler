#pragma once

#include <Tags.hpp>
#include <deque>
#include <string>
#include <vector>


enum TagParseCode
{
    NO_TAG_PARSE_ERROR,
    NO_DOCTYPE,
    HTML_MALFORMED
};

enum ScrapeCode
{
    NO_SCRAPE_ERROR,
    NO_HTML_CONTENT,
    NO_DOCTYPE_FOUND,
    MALFORMED_HTML
};


enum WriteCode
{
    NO_WRITE_ERROR,
    NO_MARKDOWN_CONTENT
};

/**
 * @class WebPage
 * @brief Represents a web page and provides methods to scrape and convert its content to markdown.
 * 
 * The WebPage class encapsulates the functionality to scrape a web page, convert its content to markdown,
 * and retrieve various attributes of the web page such as URL, title, and description.
 */
class WebPage
{
    public:

        WebPage(const char* url);
        ~WebPage();

        ScrapeCode scrape();
        WriteCode write_markdown();
        std::string get_markdown();
        std::string get_url();
        std::string get_title();
        std::string get_description();

    private:

        std::string* url;
        std::string* Title;
        std::string* Description;
        std::string* html_content;
        std::string* markdown_content;
        bool marked_down = false;
        std::vector<WebPage> sublinks;

        std::deque<Tag*> Tags;

        TagParseCode parseTagTree();

        wchar_t translate_entity_w(std::string entity);
        std::string translate_entity_s(std::string entity);
        std::string sanitize_markdown(std::string& content);


};