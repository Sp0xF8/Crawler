#pragma once

#include <deque>
#include <vector>
#include <string>
#include <unordered_map>


enum TagParseError
{
    NO_TAG_PARSE_ERROR,
    NO_DOCTYPE,
    HTML_MALFORMED
};

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


std::string tagTypeToString(TagType tag_type);
TagType stringToTagType(std::string tag_type);


struct SingleTag
{
    TagType* tag_type;
    std::string* tag;
    int* start_open;
    int* start_close;

    SingleTag(TagType& tag_type, std::string tag, int start_open, int start_close)
    {
        this->tag_type = new TagType(tag_type);
        this->tag = new std::string(tag);
        this->start_open = new int(start_open);
        this->start_close = new int(start_close);
    }

    ~SingleTag()
    {
        delete this->tag_type;
        delete this->tag;
        delete this->start_open;
        delete this->start_close;
    }
};

struct Tag
{
    TagType* Name;

    int* start_open;
    int* start_close;

    int* end_open;
    int* end_close;

    Tag* Parent;
    std::deque<Tag*> Children;


    Tag(TagType& name, int start_open, int start_close, int end_open, int end_close)
    {
        this->Name = new TagType(name);
        this->start_open = new int(start_open);
        this->start_close = new int(start_close);
        this->end_open = new int(end_open);
        this->end_close = new int(end_close);
    }

    ~Tag()
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

    void print();
    void printChildren(int indent = 0);
    std::string getContent(std::string* html_content, int indent = 0);
    static TagOrganisation getTagOrganisation(std::string* content, int start, int end);

private:
    std::string sanitizeContent(std::string& content);
    std::string beautify_content(std::string& content);
    std::string check_tabbed_only(std::string& content);


};

class WebPage
{
    public:

        WebPage(const char* url, const char* html_content);
        ~WebPage();

        void scrape();

    private:

        std::string* url;
        std::string* Title;
        std::string* Description;
        std::string* html_content;
        std::vector<WebPage> sublinks;

        std::deque<Tag*> Tags;

        TagParseError parseTagTree();

        wchar_t translate_entity_w(std::string entity);
        std::string translate_entity_s(std::string entity);
        std::string find_entites(std::string& content);
        void write_markdown(std::string& content);
        std::string sanitize_markdown(std::string& content);


};