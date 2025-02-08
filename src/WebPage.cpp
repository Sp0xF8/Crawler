#include <WebPage.hpp>
#include <Logger.hpp>
#include <cstring>
#include <sstream>


std::string tagTypeToString(TagType tag){
    return tag_to_string.at(tag);
}

TagType stringToTagType(std::string tag_type){
    if (string_to_tag.find(tag_type) == string_to_tag.end()){
        return TagType::UNKNOWN;
    }
    return string_to_tag.at(tag_type);
}


void Tag::print()
{
    LOG("Tag: ", tagTypeToString(*this->Name));
    LOG("Start Open: ", *this->start_open);
    LOG("Start Close: ", *this->start_close);
    LOG("End Open: ", *this->end_open);
    LOG("End Close: ", *this->end_close);
}

void Tag::printChildren(int indent)
{
    std::string spaces = " ";
    for (int i = 0; i < indent; i++)
    {
        spaces += " ";
    }
    LOG(spaces, "Tag: ", tagTypeToString(*this->Name));
    for (Tag *child : this->Children)
    {
        child->printChildren(indent + 2);
    }
}

std::string Tag::sanitizeContent(std::string& content){
    std::string sanitized_content = "";

    int pos = 0;

    int openTag = 0;
    int closeTag = 0;


    int opens = 0;

    while (pos < content.size()){
        openTag = content.find('<', pos);
        if (openTag == std::string::npos){

            sanitized_content += content.substr(pos);
            break;
        }

        
        closeTag = content.find('>', openTag);
        if (closeTag == std::string::npos){
            break;
        }

        TagOrganisation tag_organisation = Tag::getTagOrganisation(&content, openTag, closeTag);

        if (tag_organisation == TagOrganisation::OPENING){
            if (opens == 0){
                sanitized_content += content.substr(pos, openTag - pos);
            }

            opens++;
            
        }
        else if (tag_organisation == TagOrganisation::CLOSING){
            opens--;
            if (opens < 0){
                opens = 0;
                sanitized_content = "";
            }
 
        }
        pos = closeTag + 1;
    }

    return sanitized_content;
}

/*
*   Check if the content is tabbed only
*   For example, if the content is:
*    \t\t\t\t\n
*    or
*    \n
*    if it is tabbed content, for exmaple:A
*    \t\t\t\t[anything else]\n
*/
std::string Tag::check_tabbed_only(std::string& content){

    return "";
}

/*
*   Beautify the content:A
*   - isolate individual lines of the text
*   - remove any lines that are empty
*
*
*/
std::string Tag::beautify_content(std::string& content){
    std::string tabs_removed = "";


/// Remove every single `\t` occurance
    int pos = 0;
    int tab_pos = 0;
    while (pos < content.size()){
        tab_pos = content.find('\t', pos);
        if (tab_pos == std::string::npos){
            tabs_removed += content.substr(pos);
            break;
        }
        tabs_removed += content.substr(pos, tab_pos - pos);
        pos = tab_pos + 1;
    }


    std::string sanitized_content = "";
    //remove instances of multiple spaces in a row
    pos = 0;
    int loop = 0;

    while (pos < tabs_removed.size()){
        if (pos == tabs_removed.size() - 1){
            sanitized_content += tabs_removed.at(pos);
            break;
        }
        if (tabs_removed.at(pos) == ' '){

            if (tabs_removed.at(pos + 1) == ' ' || tabs_removed.at(pos + 1) == '\n'){
                pos++;
                continue;
            }

            
        }

        sanitized_content += tabs_removed.at(pos);
        pos++;
    }



    return sanitized_content;
}

std::string getIndentation(int depth) {
    if (depth <= 0) {
        return "";
    }
    return "  " + getIndentation(depth - 2);
}

std::string Tag::getContent(std::string* html_content, int indent){

    if (this->Children.empty())
    {        
        switch (*this->Name)
        {
            case TagType::TITLE:
            {
                return "# " + beautify_content(sanitizeContent(html_content->substr(*this->start_close + 1, *this->end_open - *this->start_close - 1))) + "\n";
            }
            case TagType::H1:
            {
                return "## " + beautify_content(sanitizeContent(html_content->substr(*this->start_close + 1, *this->end_open - *this->start_close - 1)));
            }
            case TagType::H2:
            {
                return "### " + beautify_content(sanitizeContent(html_content->substr(*this->start_close + 1, *this->end_open - *this->start_close - 1)));
            }
            case TagType::H3:
            {
                return "#### " + beautify_content(sanitizeContent(html_content->substr(*this->start_close + 1, *this->end_open - *this->start_close - 1)));
            }
            case TagType::H4:
            {
                return "##### " + beautify_content(sanitizeContent(html_content->substr(*this->start_close + 1, *this->end_open - *this->start_close - 1)));
            }
            case TagType::H5:
            {
                return "###### " + beautify_content(sanitizeContent(html_content->substr(*this->start_close + 1, *this->end_open - *this->start_close - 1)));
            }
            case TagType::H6:
            {
                return "####### " + beautify_content(sanitizeContent(html_content->substr(*this->start_close + 1, *this->end_open - *this->start_close - 1)));
            }
            case TagType::B:
            {
                return "**" + beautify_content(sanitizeContent(html_content->substr(*this->start_close + 1, *this->end_open - *this->start_close - 1))) + "**";
            }
            case TagType::I:
            {
                return "*" + beautify_content(sanitizeContent(html_content->substr(*this->start_close + 1, *this->end_open - *this->start_close - 1))) + "*";
            }
            case TagType::TH:
            {
                return "**" + beautify_content(sanitizeContent(html_content->substr(*this->start_close + 1, *this->end_open - *this->start_close - 1))) + "** ";
            }
            case TagType::TD:
            {
                return beautify_content(sanitizeContent(html_content->substr(*this->start_close + 1, *this->end_open - *this->start_close - 1)) + "\n\n");
            }
            default:
            {
                return beautify_content(sanitizeContent(html_content->substr(*this->start_close + 1, *this->end_open - *this->start_close - 1)));
                break;
            }
        }

    }
    
    std::string content = "";
    Tag* last_child = this;

    for (Tag* child : this->Children){

        std::string direct_content = html_content->substr(*last_child->start_close + 1, *child->start_open - *last_child->start_close - 1);
        direct_content = beautify_content(sanitizeContent(direct_content));
        content += direct_content;

        std::string child_content = child->getContent(html_content, indent + 2);
        content += child_content;
        last_child = child;

    }

    std::string direct_content = html_content->substr(*last_child->end_close + 1, *this->end_open - *last_child->end_close - 1);
    direct_content = beautify_content(sanitizeContent(direct_content));
    content += direct_content;

    switch (*this->Name){
        case TagType::TITLE:
        {
            return "# " + content + "\n";
        }
        case TagType::H1:
        {
            return "## " + content;
        }
        case TagType::H2:
        {
            return "### " + content;
        }
        case TagType::H3:
        {
            return "#### " + content;
        }
        case TagType::H4:
        {
            return "##### " + content;
        }
        case TagType::H5:
        {
            return "###### " + content;
        }
        case TagType::H6:
        {
            return "####### " + content;
        }
        case TagType::B:
        {
            return "**" + content + "**";
        }
        case TagType::I:
        {
            return "*" + content + "*";
        }
        case TagType::TH:
        {
            return "**" + content + "**";
        }
        case TagType::TD:
        {
            return content + "\n\n";
        }

        default:
        {
            return content;
        }
    }
}

TagOrganisation Tag::getTagOrganisation(std::string* content, int start, int end)
{
    if (content->at(start + 1) == '/')
    {
        return TagOrganisation::CLOSING;
    }
    else if (content->at(end - 1) == '/')
    {
        return TagOrganisation::SELF_CLOSING;
    }
    else
    {
        return TagOrganisation::OPENING;
    }
}

wchar_t WebPage::translate_entity_w(std::string entity){ //int start, int end){
    // // std::string entity = this->html_content->substr(start, end - start);
    std::string code = entity.substr(2, entity.size() - 1);

    if (code.at(0) == 'x'){
        code = code.substr(1);
        return std::stoi(code, nullptr, 16);
    }

    return std::stoi(code);
}

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


WebPage::WebPage(const char *url, const char *html_content)
{
    this->url = new std::string(url);
    this->html_content = new std::string(html_content);
    this->Tags = std::deque<Tag *>();
    this->Title = new std::string();
    this->Description = new std::string();

    LOG("Created WebPage object for URL: ", this->url->c_str());
}

WebPage::~WebPage()
{
    LOG("Deleted WebPage object for URL: ", this->url->c_str());
    delete this->url;
    delete this->html_content;
    delete this->Title;
    delete this->Description;

    for (Tag *tag : this->Tags)
    {
        delete tag;
    }
}

TagParseError WebPage::parseTagTree()
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
                return TagParseError::NO_DOCTYPE;
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

        // LOG("\n=========================================================");

        // LOG("Current pos: ", pos);

        // LOG("---------------------------------------------------------\n");



        // LOG("Tag stack size: ", tag_stack.size());
        // LOG("Tag branch size: ", tag_branch.size());

        // LOG ("Tag Branch:\n");
        // for (Tag* tag : tag_branch){
        //     LOG("   ", tag->Name->c_str());
        // }

        // LOG("---------------------------------------------------------\n");

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


        tag_organisation = Tag::getTagOrganisation(this->html_content, start_open, pos);
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

        // LOG(tag_type, " is ", (
        //                 tag_organisation == TagOrganisation::SELF_CLOSING ? "Self Closing" : tag_organisation == TagOrganisation::CLOSING ? "Closing"
                                                                                                                                            // : "Opening"));



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
                {
                    // LOG("Found meta tag");
                    continue;
                    break;
                }

                case TagType::TITLE:
                {
                    // LOG("Found title tag");
                    break;
                }

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

            SingleTag *sTag = new SingleTag(tag_type, tag, start_open, pos);
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
            // LOG("Self closing tag: ", tag_type);
            continue;
        }

        // ###################################################################################
        //
        //          Closing tags pop the open from the stack
        //
        // ###################################################################################

        // LOG("Closing tag: ", tag_type);

        if (tag_stack.empty())
        {
            // LOG("No opening tag to match closing tag: ", tag_type);
            continue;
        }

        SingleTag *sTag = tag_stack.front();

        // LOG("Top of stack: ", sTag->tag_type->c_str());
        // LOG("Tag ", sTag->tag->c_str());

        if (tag_type != *sTag->tag_type)
        {
            // LOG("Closing tag: ", tag_type, " does not match opening tag: ", *sTag->tag_type);
            continue;
        }
        tag_stack.pop_front();


        // LOG("Matched closing tag: ", tag_type, " with opening tag: ", *sTag->tag_type);

        Tag *new_tag = new Tag(*sTag->tag_type, *sTag->start_open, *sTag->start_close, start_open, pos);

        delete sTag;

        // LOG("Created tag: ", new_tag->Name->c_str());



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
            // LOG("Orphaned node: ", orphaned_node->Name->c_str());
            // LOG(*new_tag->start_open, " < ", *orphaned_node->start_open, " && ", *orphaned_node->end_close, " < ", *new_tag->end_close);
            if (*new_tag->start_open < *orphaned_node->start_open && *orphaned_node->end_close < *new_tag->end_close)
            {
                // LOG("New tag is parent of orphaned tag: ", orphaned_node->Name->c_str());
                new_tag->Children.push_back(orphaned_node);
                orphaned_node->Parent = new_tag;
            }
        }

        for (int i = 0; i < new_tag->Children.size(); i++){
            // LOG("Child: ", new_tag->Children[i]->Name->c_str());
            //remove child from branch
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
        return TagParseError::HTML_MALFORMED;
    }

    LOG("No unclosed tags found");
    this->Tags = tag_branch;
    return TagParseError::NO_TAG_PARSE_ERROR;

}


// remove all instances where there are more than 2 new lines
std::string WebPage::sanitize_markdown(std::string& content){

    std::string sanitized_content = "";

    int pos = 0;
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
        sanitized_content += content.at(pos);
        pos++;
    }

    return sanitized_content;
}


void WebPage::write_markdown(std::string& content){
    std::ofstream file;
    file.open("output.md");
    file << content;
    file.close();
}

std::string WebPage::find_entites(std::string& content){
    // std::string new_content = "";

    // int pos = 0;
    // int start = 0;
    // int end = 0;

    // start = content.find('&', pos);

    // if (start == std::string::npos){
    //     return content;
    // }

    // new_content += content.substr(pos, start - pos);


    // while (pos < content.size()){


    //     end = content.find(';', start);
    //     if (end == std::string::npos){
    //         new_content += content.substr(pos);
    //         break;
    //     }

    //     if (end - start > 10){
    //         new_content += content.substr(pos, start - pos);
    //         pos = start + 1;
    //         continue;
    //     }

    //     std::string entity = content.substr(start, end - start + 1);
    //     new_content += translate_entity_s(entity);

    //     start = content.find('&', end + 1);
    //     if (start != std::string::npos){
    //         new_content += content.substr(end + 1, start - end - 1);
    //         pos = end + 1;
    //         continue;
    //     }       
    //     if (start == std::string::npos){
    //         new_content += content.substr(end + 1);
    //         break;
    //     }
       
    // }

    // return new_content;
    return content;
}


void WebPage::scrape()
{

    if (this->html_content == nullptr)
    {
        LOG("No HTML content to scrape");
        return;
    }



    LOG("Scraping URL: ", this->url->c_str());

    LOG("HTML Content: ", this->html_content->c_str());


    // std::deque<Tag*> tag_tree;
    TagParseError error = this->parseTagTree();


    switch (error)
    {
        case TagParseError::NO_DOCTYPE:
        {
            LOG("No DOCTYPE found");
            return;
            break;
        }

        case TagParseError::HTML_MALFORMED:
        {
            LOG("HTML is malformed, unable to generate full tree");
            //being cool and not breaking to abuse the default call
        }
        default:
        {
            // for (Tag* tag : this->Tags){
            //     tag->printChildTags();
            // }

            std:: string content = "";
            for (Tag* tag : this->Tags){
                content += tag->getContent(this->html_content);
            }
            LOG("Content: ", content);

            content = "# " + *this->url + "\n\n" + content;
            this->write_markdown(find_entites(sanitize_markdown(content)));
            break;
        }
    }

    // wchar_t translated = this->translate_entity_w("&#167;");
    // WLOG("Translated: ", translated);
    std::string utf8 = this->translate_entity_s("&#167;");
    LOG("UTF-8: ", utf8);
    LOG(getIndentation(10), "Hello");

    LOG("Finished scraping URL: ", this->url->c_str());
}