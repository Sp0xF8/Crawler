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

std::string getIndentation(int depth) {
    if (depth <= 0) {
        return "";
    }
    return "  " + getIndentation(depth - 2);
}

std::string Tag::getContent(std::string* html_content, int indent){

    if (this->Children.empty())
    {        
        return beautify_content(sanitizeContent(html_content->substr(*this->start_close + 1, *this->end_open - *this->start_close - 1)));
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

    return content;
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



/*
*   Beautify the content:A
*   - isolate individual lines of the text
*   - remove any lines that are empty
*
*
*/
std::string Tag::beautify_content(std::string& content){
    std::string beautified_content = "";

    std::string line = "";


    int line_start = content.find('\n', 0);
    if (line_start == std::string::npos){
        return content;
    }
    if (line_start > 0){
        beautified_content += content.substr(0, line_start);
    }

    int line_end = 0;
    bool tabbed = true;
    bool only_enter = false;
    int deepest_tab = 0;
    int deepest_enter = 0;
    int deepest_non_enter = 0;


    while (line_start < content.size()){
        line_end = content.find('\n', line_start + 1);
        if (line_end == std::string::npos){
            line = content.substr(line_start);
            if (line.size() > 0){
                beautified_content += line;
            }
            break;
        }
        
        if (line_start == line_end - 1){
            line_start = line_end + 1;
            continue;
        }

        line = content.substr(line_start, line_end - line_start + 1);

        for (int i = 0; i < line.size(); i++){
            if (line[i] == '\t'){
                deepest_tab = i;
                continue;
            }

            if ((line[i] != '\t' && line[i] != '\n') && i > deepest_non_enter){
                deepest_non_enter = i;
                break;
            }
        }

        // for (int i = 0; i < line.size(); i++){
        //     if (line[i] == '\n'){
        //         deepest_enter = i;
        //         continue;
        //     }

        //     if (line[i] != '\n' && i > deepest_enter){
        //         deepest_non_enter = i;
        //         continue;
        //     }

          
        // }

        if (deepest_tab < deepest_non_enter){
            beautified_content += line;
        }


        line_start = line_end;
       
    }

    return beautified_content;
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