#pragma once

#include <deque>
#include <vector>
#include <string>

enum TagOrganisation
{
    None,
    SELF_CLOSING,
    CLOSING,
    OPENING
};

// enum TagType
// {
//     HTML,
//     HEAD,
//     BODY,
//     TITLE,
//     META,
//     LINK,
//     SCRIPT,
//     STYLE,
//     DIV,
//     SPAN,
//     P,
//     H1,
//     H2,
//     H3,
//     H4,
//     H5,
//     H6,
//     A,
//     IMG,
//     UL,
//     OL,
//     LI,
//     TABLE,
//     TR,
//     TH,
//     TD,
//     FORM,
//     INPUT,
//     BUTTON,
//     SELECT,
//     OPTION,
//     TEXTAREA,
//     G,
//     TEXT,
//     TSPAN,
//     TEXT_PATH,
//     IMAGE,
//     AUDIO,
//     VIDEO
// };

struct Tag
{
    std::string* Name;

    int* start_open;
    int* start_close;

    int* end_open;
    int* end_close;

    std::deque<Tag*> Parents;
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

};

struct PageData
{
    std::string* Title;
    std::string* Description;
    std::deque<Tag*> Tags;

    PageData()
    {
        this->Title = new std::string();
        this->Description = new std::string();
    }

    ~PageData()
    {
        delete this->Title;
        delete this->Description;

        for (Tag* tag : this->Tags)
        {
            delete tag;
        }

        this->Tags.clear();
    }
};

class WebPage
{
    public:

        WebPage(const char* url, const char* html_content);
        ~WebPage();

        void scrape();

    private:

        std::vector<WebPage> sublinks;
        std::string* url;
        std::string* html_content;
        PageData* page_data;


        

};