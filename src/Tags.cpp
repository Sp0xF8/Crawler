#include <Tags.hpp>
#include <Logger.hpp>

/**
 * @brief A constant unordered map that associates HTML tag names with their corresponding TagType enum values.
 * 
 * This map is used to convert string representations of HTML tags into their respective TagType enum values.
 * 
 * Example usage:
 * 
 * @code
 * TagType tagType = string_to_tag.at("div"); // tagType will be TagType::DIV
 * @endcode
 * 
 */
const std::unordered_map<std::string, TagType> string_to_tag = {
    {"!DOCTYPE", DOCTYPE},
    {"html", HTML},
    {"head", HEAD},
    {"title", TITLE},
    {"meta", META},
    {"body", BODY},
    {"p", P},
    {"b", B},
    {"i", I},
    {"h1", H1},
    {"h2", H2},
    {"h3", H3},
    {"h4", H4},
    {"h5", H5},
    {"h6", H6},
    {"a", A},
    {"img", IMG},
    {"div", DIV},
    {"span", SPAN},
    {"ul", UL},
    {"ol", OL},
    {"li", LI},
    {"table", TABLE},
    {"tr", TR},
    {"th", TH},
    {"td", TD},
    {"form", FORM},
    {"label", LABEL},
    {"input", INPUT__TEXT},
    {"button", BUTTON},
    {"select", SELECT},
    {"option", OPTION},
    {"textarea", TEXTAREA},
    {"script", SCRIPT},
    {"style", STYLE},
    {"link", LINK},
    {"br", BR},
    {"hr", HR},
    {"!--", COMMENT},
    {"unknown", UNKNOWN},
    {"cite", CITE},
    {"font", FONT}
};

/**
 * @brief A mapping from TagType enum values to their corresponding HTML tag strings.
 * 
 * This unordered_map provides a way to convert TagType enum values to their
 * string representations as used in HTML. It includes common HTML tags such as
 * "html", "head", "body", "p", "a", "img", and many others.
 * 
 * Example usage:
 * 
 * @code
 * std::string tagName = tag_to_string[TagType::HTML]; // tagName will be "html"
 * @endcode
 * 
 */
const std::unordered_map<TagType, std::string> tag_to_string = {
    {DOCTYPE, "!DOCTYPE"},
    {HTML, "html"},
    {HEAD, "head"},
    {TITLE, "title"},
    {META, "meta"},
    {BODY, "body"},
    {P, "p"},
    {B, "b"},
    {I, "i"},
    {H1, "h1"},
    {H2, "h2"},
    {H3, "h3"},
    {H4, "h4"},
    {H5, "h5"},
    {H6, "h6"},
    {A, "a"},
    {IMG, "img"},
    {DIV, "div"},
    {SPAN, "span"},
    {UL, "ul"},
    {OL, "ol"},
    {LI, "li"},
    {TABLE, "table"},
    {TR, "tr"},
    {TH, "th"},
    {TD, "td"},
    {FORM, "form"},
    {LABEL, "label"},
    {INPUT__TEXT, "input"},
    {BUTTON, "button"},
    {SELECT, "select"},
    {OPTION, "option"},
    {TEXTAREA, "textarea"},
    {SCRIPT, "script"},
    {STYLE, "style"},
    {LINK, "link"},
    {BR, "br"},
    {HR, "hr"},
    {COMMENT, "!--"},
    {UNKNOWN, "unknown"},
    {CITE, "cite"},
    {FONT, "font"}
};

/**
 * @brief Converts a TagType enum value to its corresponding string representation.
 * 
 * @param tag The TagType enum value to be converted.
 * @return std::string The string representation of the given TagType.
 * 
 * @throws std::out_of_range if the given TagType is not found in the tag_to_string map.
 */
std::string tagTypeToString(TagType tag){
    try{
        return tag_to_string.at(tag);
    }
    catch (std::out_of_range& e){
        LOG("TagType not found in tag_to_string map");
        return "UNKNOWN";
    }
}

/**
 * @brief Converts a string representation of a tag type to its corresponding TagType enum value.
 *
 * This function attempts to find the TagType associated with the given string in the string_to_tag map.
 * If the string is not found in the map, it logs an error message and returns TagType::UNKNOWN.
 *
 * @param tag_type The string representation of the tag type.
 * @return The corresponding TagType enum value, or TagType::UNKNOWN if the string is not found.
 */
TagType stringToTagType(std::string tag_type){
    try{
        return string_to_tag.at(tag_type);
    }
    catch (std::out_of_range& e){
        LOG("TagType not found in string_to_tag map");
        return TagType::UNKNOWN;
    }

}


/**
 * @brief Constructs a SingleTag object.
 * @param tag_type The type of the tag.
 * @param start_open The start position of the opening tag.
 * @param start_close The start position of the closing tag.
 */
SingleTag::SingleTag(TagType& tag_type, int start_open, int start_close)
{
    this->tag_type = new TagType(tag_type);
    this->start_open = new int(start_open);
    this->start_close = new int(start_close);
}

/**
 * @brief Destructor for the SingleTag class.
 *
 * This destructor is responsible for cleaning up the dynamically allocated
 * memory for the tag_type, start_open, and start_close members of the SingleTag
 * class to prevent memory leaks.
 */
SingleTag::~SingleTag()
{
    delete this->tag_type;
    delete this->start_open;
    delete this->start_close;
}

/**
 * @brief Determines the type of tag organisation based on the content and specified range.
 * 
 * This function inspects the characters at the start and end positions within the given content
 * to determine whether the tag is an opening tag, a closing tag, or a self-closing tag.
 * 
 * @param content A pointer to the string containing the tag content.
 * @param start The starting index of the tag within the content.
 * @param end The ending index of the tag within the content.
 * @return TagOrganisation The type of tag organisation (OPENING, CLOSING, SELF_CLOSING).
 */
TagOrganisation SingleTag::getTagOrganisation(std::string* content, int start, int end)
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


// void Tag::print()
// {
//     LOG("Tag: ", tagTypeToString(*this->Name));
//     LOG("Start Open: ", *this->start_open);
//     LOG("Start Close: ", *this->start_close);
//     LOG("End Open: ", *this->end_open);
//     LOG("End Close: ", *this->end_close);
// }

// void Tag::printChildren(int indent)
// {
//     std::string spaces = " ";
//     for (int i = 0; i < indent; i++)
//     {
//         spaces += " ";
//     }
//     LOG(spaces, "Tag: ", tagTypeToString(*this->Name));
//     for (Tag *child : this->Children)
//     {
//         child->printChildren(indent + 2);
//     }
// }



/**
 * @brief Constructs a new Tag object.
 * 
 * @param name The name of the tag, passed by reference.
 * @param start_open The starting position of the opening tag.
 * @param start_close The ending position of the opening tag.
 * @param end_open The starting position of the closing tag.
 * @param end_close The ending position of the closing tag.
 */
Tag::Tag(TagType& name, int start_open, int start_close, int end_open, int end_close)
{
    this->Name = new TagType(name);
    this->start_open = new int(start_open);
    this->start_close = new int(start_close);
    this->end_open = new int(end_open);
    this->end_close = new int(end_close);
}

/**
 * @brief Destructor for the Tag class.
 *
 * This destructor is responsible for cleaning up the dynamically allocated
 * memory associated with the Tag object. It deletes the memory allocated for
 * the Name, start_open, start_close, end_open, and end_close members. It also
 * iterates through the Children vector and deletes each child Tag object before
 * clearing the vector.
 */
Tag::~Tag()
{
    delete this->Name;
    delete this->start_open;
    delete this->start_close;
    delete this->end_open;
    delete this->end_close;

    for (Tag* child : this->Children)
    {
        delete child;
    }

    this->Children.clear();
}

/**
 * @brief Sanitizes the content by removing HTML-like tags.
 *
 * This function processes the input string and removes any HTML-like tags,
 * preserving the text content outside of these tags. It handles nested tags
 * and ensures that only the text outside the outermost tags is kept.
 *
 * @param content The input string containing the content to be sanitized.
 * @return A sanitized string with HTML-like tags removed.
 */
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

        TagOrganisation tag_organisation = SingleTag::getTagOrganisation(&content, openTag, closeTag);

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


/**
 * @brief Beautifies the given content by removing tabs and reducing multiple spaces.
 *
 * This function processes the input content string by performing the following steps:
 * 1. Removes all occurrences of the tab character (`\t`).
 * 2. Reduces multiple consecutive spaces to a single space.
 * 3. Removes spaces that are followed by a newline character (`\n`).
 *
 * @param content The input string to be beautified.
 * @return A new string with the beautified content.
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

/**
 * @brief Parses the content based on the tag type and returns the formatted string.
 * 
 * This function takes a reference to a string containing the content and formats it
 * according to the tag type specified in the `Name` member of the `Tag` class. The
 * formatting is done as follows:
 * - TagType::TITLE: Adds a single '#' prefix and a newline suffix.
 * - TagType::H1: Adds a '##' prefix.
 * - TagType::H2: Adds a '###' prefix.
 * - TagType::H3: Adds a '####' prefix.
 * - TagType::H4: Adds a '#####' prefix.
 * - TagType::H5: Adds a '######' prefix.
 * - TagType::H6: Adds a '#######' prefix.
 * - TagType::B: Encloses the content in double asterisks '**'.
 * - TagType::I: Encloses the content in single asterisks '*'.
 * - TagType::TH: Encloses the content in double asterisks '**' and adds a space suffix.
 * - TagType::TD: Adds two newline characters '\n\n' as a suffix.
 * - Default: Returns the content as is.
 * 
 * @param content A reference to the string containing the content to be formatted.
 * @return A formatted string based on the tag type.
 */
std::string Tag::parse_content(std::string& content){
    switch (*this->Name)
    {
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
            return "**" + content + "** ";
        }
        case TagType::TD:
        {
            return content +"\n\n";
        }
        default:
        {
            return content;
            break;
        }
    }
}

/**
 * @brief Extracts and processes the content within the tag.
 * 
 * This function retrieves the content between the start and end tags, processes it by sanitizing and beautifying,
 * and then parses the content. If the tag has child tags, it recursively processes the content of each child tag.
 * 
 * @param html_content Pointer to the HTML content string.
 * @param indent The indentation level for beautifying the content.
 * @return A string containing the processed content of the tag.
 */
std::string Tag::getContent(std::string* html_content, int indent){

    if (this->Children.empty())
    {        
        
        return this->parse_content(beautify_content(sanitizeContent(html_content->substr(*this->start_close + 1, *this->end_open - *this->start_close - 1))));
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

    return parse_content(content);
}
