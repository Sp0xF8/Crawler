#include <WebPage.hpp>
#include <Logger.hpp>
#include <CurlManager.hpp>
#include <cstring>
#include <sstream>



/**
 * @brief Translates an HTML entity to its corresponding wide character.
 *
 * This function takes an HTML entity in the form of a string and converts it
 * to its corresponding wide character. The entity is expected to be in the
 * format "&#NNNN;" or "&#xHHHH;", where NNNN is a decimal number and HHHH is
 * a hexadecimal number.
 *
 * @param entity The HTML entity string to be translated.
 * @return The wide character corresponding to the HTML entity.
 */
wchar_t WebPage::translate_entity_w(std::string entity){ //int start, int end){
    // // std::string entity = this->html_content->substr(start, end - start);
    std::string code = entity.substr(2, entity.size() - 1);

    if (code.at(0) == 'x'){
        code = code.substr(1);
        return std::stoi(code, nullptr, 16);
    }

    return std::stoi(code);
}


/**
 * @brief Translates an HTML entity to its corresponding UTF-8 character.
 *
 * This function takes an HTML entity in the form of a string (e.g., "&#x2009;" or "&#8201;")
 * and converts it to its corresponding UTF-8 character.
 *
 * @param entity The HTML entity to be translated. It should be in the form of "&#xHHHH;" for
 *               hexadecimal entities or "&#DDDD;" for decimal entities.
 * @return A string containing the UTF-8 character corresponding to the given HTML entity.
 *         If the entity is not valid, the original entity string is returned.
 */
std::string WebPage::translate_entity_s(std::string entity){

    int codePoint = 0;
    if (entity[2] == 'x' || entity[2] == 'X') { // Hexadecimal entity (&#x2009;)
        std::stringstream ss(entity.substr(3, entity.size() - 4)); // Extract hex part
        ss >> std::hex >> codePoint;
    } else if (entity[1] != '#') { // Not a valid entity
        return entity;
    } else { // Decimal entity (&#8201;)
        codePoint = std::stoi(entity.substr(2, entity.size() - 3));
    }

    // Convert to UTF-8 string
    std::string utf8Char;
    if (codePoint <= 0x7F) { // 1-byte UTF-8
        utf8Char += static_cast<char>(codePoint);
    } else if (codePoint <= 0x7FF) { // 2-byte UTF-8
        utf8Char += static_cast<char>(0xC0 | ((codePoint >> 6) & 0x1F));
        utf8Char += static_cast<char>(0x80 | (codePoint & 0x3F));
    } else if (codePoint <= 0xFFFF) { // 3-byte UTF-8
        utf8Char += static_cast<char>(0xE0 | ((codePoint >> 12) & 0x0F));
        utf8Char += static_cast<char>(0x80 | ((codePoint >> 6) & 0x3F));
        utf8Char += static_cast<char>(0x80 | (codePoint & 0x3F));
    } else { // 4-byte UTF-8
        utf8Char += static_cast<char>(0xF0 | ((codePoint >> 18) & 0x07));
        utf8Char += static_cast<char>(0x80 | ((codePoint >> 12) & 0x3F));
        utf8Char += static_cast<char>(0x80 | ((codePoint >> 6) & 0x3F));
        utf8Char += static_cast<char>(0x80 | (codePoint & 0x3F));
    }

    return utf8Char;
}


/**
 * @brief Constructs a new WebPage object.
 * 
 * This constructor initializes a WebPage object with the given URL. It fetches the HTML content
 * from the URL using the curl_manager, initializes an empty deque for Tags, and allocates memory
 * for the Title and Description strings. The markdown_content is initialized to nullptr.
 * 
 * @param url The URL of the web page to be fetched and processed.
 */
WebPage::WebPage(const char *url)
{
    this->url = new std::string(url);
    this->html_content = curl_manager.get(url);
    this->Tags = std::deque<Tag *>();
    this->Title = new std::string();
    this->Description = new std::string();
    this->markdown_content = nullptr;

    LOG("Created WebPage object for URL: ", this->url->c_str());
}

/**
 * @brief Destructor for the WebPage class.
 *
 * This destructor is responsible for cleaning up the dynamically allocated memory
 * associated with a WebPage object. It performs the following actions:
 * - Logs the deletion of the WebPage object along with its URL.
 * - Deletes the dynamically allocated URL string.
 * - Deletes the dynamically allocated HTML content string.
 * - Deletes the dynamically allocated Title string.
 * - Deletes the dynamically allocated Description string.
 * - Deletes the dynamically allocated markdown content string if it is not nullptr.
 * - Iterates through the list of Tags and deletes each Tag object.
 */
WebPage::~WebPage()
{
    LOG("Deleted WebPage object for URL: ", this->url->c_str());
    delete this->url;
    delete this->html_content;
    delete this->Title;
    delete this->Description;

    if (this->markdown_content != nullptr)
    {
        delete this->markdown_content;
    }

    for (Tag *tag : this->Tags)
    {
        delete tag;
    }
}

/**
 * @brief Parses the HTML content of the WebPage to construct a tree of tags.
 * 
 * This function scans through the HTML content of the WebPage, identifies tags,
 * and organizes them into a tree structure. It handles different types of tags
 * including opening, closing, and self-closing tags. The function also checks
 * for the presence of a DOCTYPE declaration and logs appropriate messages if
 * it is not found.
 * 
 * @return TagParseCode 
 * - NO_DOCTYPE: If no DOCTYPE declaration is found.
 * - HTML_MALFORMED: If there are unclosed tags in the HTML content.
 * - NO_TAG_PARSE_ERROR: If the HTML content is parsed successfully without errors.
 */
TagParseCode WebPage::parseTagTree()
{
    
    int start = -1;

    std::string tag;
    TagType tag_type;

    int start_open = 0;
    // int start_close = 0;

    int next_space = 0;
    int next_forward_slash = 0;
    TagOrganisation tag_organisation = TagOrganisation::OPENING;


    size_t pos = 0;
    pos = this->html_content->find("<!doctype html", 0);
    if (pos == std::string::npos)
    {
        LOG("<!doctype html> not found, WebPage not parseable");

        pos = this->html_content->find("<!DOCTYPE html", 0);
        if (pos == std::string::npos)
        {
            LOG("<!DOCTYPE html> not found, WebPage not parseable");

            pos = this->html_content->find("<!DOCTYPE HTML", 0);
            if (pos == std::string::npos)
            {
                LOG("<!DOCTYPE HTML> not found, WebPage not parseable");
                return TagParseCode::NO_DOCTYPE;
            }
        }
    }

    pos += 0xf;

    int start_addition = 0;
    int end_negation = 0;

    std::deque<SingleTag *> tag_stack;

    std::deque<Tag *> tag_branch;


    while (pos < this->html_content->size())
    {
        // #########################################################################################
        //
        //           Reset variables for next iteration
        //
        // #########################################################################################

        start_addition = 0;
        end_negation = 0;

        tag_organisation = TagOrganisation::None;

        // #########################################################################################
        //
        //           Find Tags
        //
        // #########################################################################################


        if (this->html_content->at(pos) != '<')
        {
            pos = this->html_content->find('<', pos);
            if (pos == std::string::npos)
            {
                LOG("No more tags found");
                break;
            }
        }

        // #########################################################################################
        //
        //           Find Tag organisation (opening, closing, self-closing)
        //                 + Calculate offset for tag type
        //
        // #########################################################################################

        start_open = pos;
        pos = this->html_content->find('>', pos);
        tag = this->html_content->substr(start_open, pos - start_open + 1);
        // LOG("Found tag: ", tag, " at pos: ", start_open, " to ", pos);


        tag_organisation = SingleTag::getTagOrganisation(this->html_content, start_open, pos);
        if (tag_organisation == TagOrganisation::CLOSING)
        {
            start_addition = 1;
        }
        else if (tag_organisation == TagOrganisation::SELF_CLOSING)
        {
            end_negation = 1;
        }

        // ###################################################################################
        //
        //          Find tag type

        next_space = this->html_content->find(' ', start_open); 

        if (next_space == std::string::npos || next_space > pos)
        {
            tag_type = stringToTagType(this->html_content->substr(start_open + 1 + start_addition, pos - start_open - start_addition - 1 - end_negation));
        }
        else
        {
            tag_type = stringToTagType(this->html_content->substr(start_open + 1 + start_addition, next_space - start_open - 1 - start_addition));
        }

        // ###################################################################################
        //
        //          Push the found tag to the stack
        //
        // ###################################################################################

        if (tag_organisation == TagOrganisation::OPENING){
            // LOG("Pushed opening tag");

            switch (tag_type)
            {

                case TagType::META:
                case TagType::LINK:
                case TagType::IMG:
                case TagType::INPUT__TEXT:
                case TagType::BUTTON:
                case TagType::SELECT:
                case TagType::OPTION:
                case TagType::CITE:
                case TagType::SCRIPT:
                case TagType::STYLE:
                case TagType::BR:
                case TagType::HR:
                case TagType::COMMENT:
                case TagType::UNKNOWN:
                {
                    // LOG("Found tag: ", tag_type);
                    continue;
                    break;
                }

                default:
                {
                    break;
                }

            }

            SingleTag *sTag = new SingleTag(tag_type, start_open, pos);
            tag_stack.push_front(sTag);
            continue;
        }

        // ###################################################################################
        //
        //          Exit Early from function as self closing tags hold no text
        //
        // ###################################################################################


        if (tag_organisation == TagOrganisation::SELF_CLOSING)
        {
            //Handle self closing tags that contain links

            continue;
        }

        // ###################################################################################
        //
        //          Closing tags pop the open from the stack
        //
        // ###################################################################################


        if (tag_stack.empty())
        {
            continue;
        }

        SingleTag *sTag = tag_stack.front();


        if (tag_type != *sTag->tag_type)
        {
            continue;
        }
        tag_stack.pop_front();


        Tag *new_tag = new Tag(*sTag->tag_type, *sTag->start_open, *sTag->start_close, start_open, pos);

        delete sTag;

        // ###################################################################################
        //
        //          Add tag to the tree
        //
        // ###################################################################################


        if (tag_branch.empty()) // only happens for the first tag
        {
            tag_branch.push_back(new_tag);
            continue;
        }

        for (Tag* orphaned_node : tag_branch){

            if (*new_tag->start_open < *orphaned_node->start_open && *orphaned_node->end_close < *new_tag->end_close)
            {
                new_tag->Children.push_back(orphaned_node);
                orphaned_node->Parent = new_tag;
            }
        }

        for (int i = 0; i < new_tag->Children.size(); i++){

            auto it = std::find(tag_branch.begin(), tag_branch.end(), new_tag->Children[i]);
            if (it != tag_branch.end()) {
                tag_branch.erase(it);
            }
        }

        tag_branch.push_back(new_tag);
    }

    if (!tag_stack.empty())
    {
        LOG("Unclosed tags found");

        for (SingleTag *sTag : tag_stack)
        {
            LOG("Unclosed tag: ", tagTypeToString(*sTag->tag_type));
            delete sTag;
        }
        this->Tags = tag_branch;
        return TagParseCode::HTML_MALFORMED;
    }

    LOG("No unclosed tags found");
    this->Tags = tag_branch;
    return TagParseCode::NO_TAG_PARSE_ERROR;

}



/**
 * @brief Sanitizes the given markdown content by removing excessive new lines and translating HTML entities.
 *
 * This function processes the input markdown content to ensure that there are no more than two consecutive new lines.
 * Additionally, it translates HTML entities (e.g., &amp;, &lt;, &gt;) into their corresponding characters.
 *
 * @param content The markdown content to be sanitized. This parameter is passed by reference.
 * @return A sanitized version of the input markdown content.
 */
std::string WebPage::sanitize_markdown(std::string& content){

    std::string sanitized_content = "";

    int pos = 0;
    int end = 0;
    int new_lines = 0;

    while (pos < content.size()){
        if (content.at(pos) == '\n'){
            new_lines++;
            if (new_lines > 2){
                pos++;
                continue;
            }
        }
        else{
            if (new_lines == 1){
                sanitized_content += "\n";
            }
            new_lines = 0;
        }


        if (content.at(pos) != '&'){
            sanitized_content += content.at(pos);
            pos++;
            continue;
        }


        end = 0;
        for (int i = pos; i < content.size() && !(i - pos > 10); i++){
            if (content.at(i) == ';'){
                end = i;
                break;
            }
        }

        if (end == 0){
            sanitized_content += content.at(pos);
            pos++;
            continue;
        }

        std::string entity = content.substr(pos, end - pos + 1);
        sanitized_content += translate_entity_s(entity);
        pos = end + 1;
    }

    return sanitized_content;
}


/**
 * @brief Writes the markdown content of the WebPage to a file.
 *
 * This function checks if the WebPage object has markdown content. If the content is present,
 * it writes the content to a file named "output.md". If the content is not present, it logs
 * an error message and returns an appropriate error code.
 *
 * @return WriteCode::NO_MARKDOWN_CONTENT if there is no markdown content to write.
 *         WriteCode::NO_WRITE_ERROR if the markdown content is successfully written to the file.
 */
WriteCode WebPage::write_markdown(){

    if (this->markdown_content == nullptr){
        LOG("No markdown content to write");
        return WriteCode::NO_MARKDOWN_CONTENT;
    }

    std::ofstream file;
    file.open("output.md");
    file << *this->markdown_content;
    file.close();

    return WriteCode::NO_WRITE_ERROR;
}


/**
 * @brief Scrapes the HTML content of the web page and converts it to markdown.
 * 
 * This function checks if the HTML content is available, logs the scraping process,
 * parses the tag tree, and converts the content to markdown format. It handles
 * different parsing errors and returns appropriate scrape codes.
 * 
 * @return ScrapeCode indicating the result of the scraping process.
 *         - ScrapeCode::NO_HTML_CONTENT: No HTML content to scrape.
 *         - ScrapeCode::NO_DOCTYPE_FOUND: No DOCTYPE found in the HTML.
 *         - ScrapeCode::MALFORMED_HTML: HTML is malformed and unable to generate a full tree.
 *         - ScrapeCode::NO_SCRAPE_ERROR: Scraping completed successfully without errors.
 */
ScrapeCode WebPage::scrape()
{

    if (this->html_content == nullptr)
    {
        LOG("No HTML content to scrape");
        return ScrapeCode::NO_HTML_CONTENT;
    }


    LOG("Scraping URL: ", this->url->c_str());

    TagParseCode error = this->parseTagTree();

    switch (error)
    {
        case TagParseCode::NO_DOCTYPE:
        {
            LOG("No DOCTYPE found");
            return ScrapeCode::NO_DOCTYPE_FOUND;
            break;
        }

        case TagParseCode::HTML_MALFORMED:
        {
            LOG("HTML is malformed, unable to generate full tree");
            //being cool and not breaking to abuse the default call

        }
        default:
        {
            std::string content = "";
            for (Tag* tag : this->Tags){
                content += tag->getContent(this->html_content);
            }
            content = "# URL: \n- " + *this->url + "\n\n" + content;
            this->markdown_content = new std::string(sanitize_markdown(content));
            this->marked_down = true;
            break;
        }
    }

    LOG("Finished scraping URL: ", this->url->c_str());

    if (error == TagParseCode::HTML_MALFORMED){
        return ScrapeCode::MALFORMED_HTML;
    }

    return ScrapeCode::NO_SCRAPE_ERROR;
}

/**
 * @brief Retrieves the markdown content of the web page.
 *
 * This function checks if the web page has been marked down. If it has,
 * it returns the markdown content as a string. If the web page has not
 * been marked down, it returns an empty string.
 *
 * @return std::string The markdown content of the web page if it exists,
 *         otherwise an empty string.
 */
std::string WebPage::get_markdown()
{
    if (this->marked_down)
    {
        return std::string(*this->markdown_content);
    }

    return "";
}

/**
 * @brief Retrieves the URL of the web page.
 * 
 * @return std::string The URL of the web page.
 */
std::string WebPage::get_url()
{
    return std::string(*this->url);
}

/**
 * @brief Retrieves the title of the web page.
 * 
 * This function returns the title of the web page as a std::string.
 * 
 * @return std::string The title of the web page.
 */
std::string WebPage::get_title()
{
    return std::string(*this->Title);
}

/**
 * @brief Retrieves the description of the web page.
 * 
 * This function returns a string that contains the description of the web page.
 * The description is stored in the member variable `Description`.
 * 
 * @return std::string The description of the web page.
 */
std::string WebPage::get_description()
{
    return std::string(*this->Description);
}
