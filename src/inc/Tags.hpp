#pragma once

#include <string>
#include <unordered_map>
#include <deque>

enum TagOrganisation
{
    None,
    SELF_CLOSING,
    CLOSING,
    OPENING
};

enum TagType
{
    DOCTYPE,
    HTML,
    HEAD,
    TITLE,
    META,
    BODY,
    P,
    B,
    I,
    H1,
    H2,
    H3,
    H4,
    H5,
    H6,
    A,
    IMG,
    DIV,
    SPAN,
    UL,
    OL,
    LI,
    TABLE,
    TR,
    TH,
    TD,
    FORM,
    LABEL,
    INPUT__TEXT,
    BUTTON,
    SELECT,
    OPTION,
    TEXTAREA,
    SCRIPT,
    STYLE,
    LINK,
    BR,
    HR,
    COMMENT,
    UNKNOWN,
    CITE,
    FONT
};


std::string tagTypeToString(TagType tag_type);
TagType stringToTagType(std::string tag_type);

/**
 * @struct SingleTag
 * @brief Represents a single HTML tag with its type and position.
 */
struct SingleTag
{
    TagType* tag_type;       /**< The type of the tag */
    int* start_open;         /**< The start position of the opening tag */
    int* start_close;        /**< The start position of the closing tag */

    SingleTag(TagType& tag_type, int start_open, int start_close);
    ~SingleTag();

    static TagOrganisation getTagOrganisation(std::string* content, int start, int end);
};

/**
 * @struct Tag
 * @brief Represents an HTML tag with its type, position, parent, and children.
 */
struct Tag
{
    TagType* Name;               /**< The name/type of the tag */
    int* start_open;             /**< The start position of the opening tag */
    int* start_close;            /**< The start position of the closing tag */
    int* end_open;               /**< The end position of the opening tag */
    int* end_close;              /**< The end position of the closing tag */
    Tag* Parent;                 /**< The parent tag */
    std::deque<Tag*> Children;   /**< The child tags */


    Tag(TagType& name, int start_open, int start_close, int end_open, int end_close);
    ~Tag();

    std::string getContent(std::string* html_content, int indent = 0);

private:

    std::string sanitizeContent(std::string& content);
    std::string beautify_content(std::string& content);
    std::string parse_content(std::string& content);
};


