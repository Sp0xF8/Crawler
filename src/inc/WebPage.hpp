#pragma once

#include <deque>
#include <vector>
#include <string>


enum TagParseError
{
    NO_TAG_PARSE_ERROR,
    NO_DOCTYPE
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
    COMMENT
};

struct SingleTag
{
    std::string *tag_type;
    std::string *tag;
    int *start_open;
    int *start_close;

    SingleTag(std::string tag_type, std::string tag, int start_open, int start_close)
    {
        this->tag_type = new std::string(tag_type);
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
    std::string* Name;

    int* start_open;
    int* start_close;

    int* end_open;
    int* end_close;

    Tag* Parent;
    std::deque<Tag*> Children;


    Tag(std::string name, int start_open, int start_close, int end_open, int end_close)
    {
        this->Name = new std::string(name);
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

};

// struct PageData
// {
//     std::string* Title;
//     std::string* Description;
//     std::deque<Tag*> Tags;

//     PageData()
//     {
//         this->Title = new std::string();
//         this->Description = new std::string();
//         this->Tags = std::deque<Tag*>();
//     }

//     ~PageData()
//     {
//         delete this->Title;
//         delete this->Description;

//         for (Tag* tag : this->Tags)
//         {
//             delete tag;
//         }

//         this->Tags.clear();
//     }
// };

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
        // PageData* page_data;
        std::deque<Tag*> Tags;

        TagParseError parseTagTree();


};