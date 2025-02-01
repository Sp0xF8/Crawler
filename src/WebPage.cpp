#include <WebPage.hpp>
#include <Logger.hpp>
#include <cstring>

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

WebPage::WebPage(const char *url, const char *html_content)
{
    this->url = new std::string(url);
    this->html_content = new std::string(html_content);
    this->page_data = new PageData();

    LOG("Created WebPage object for URL: ", this->url->c_str());
}

WebPage::~WebPage()
{
    LOG("Deleted WebPage object for URL: ", this->url->c_str());
    delete this->url;
    delete this->html_content;
    delete this->page_data;
}

void WebPage::scrape()
{

    if (this->html_content == nullptr)
    {
        LOG("No HTML content to scrape");
        return;
    }

    if (this->page_data != nullptr)
    {
        delete this->page_data;
        this->page_data = nullptr;
    }

    LOG("Scraping URL: ", this->url->c_str());

    LOG("HTML Content: ", this->html_content->c_str());

    int start = -1;

    std::string tag;
    std::string tag_type;

    int start_open = 0;
    // int start_close = 0;

    int next_space = 0;
    int next_forward_slash = 0;
    TagOrganisation tag_organisation = TagOrganisation::OPENING;

    std::string search_str = "<!doctype html>";

    size_t pos = 0;

    pos = this->html_content->find(search_str, pos);
    if (pos == std::string::npos)
    {
        LOG("Doctype not found, WebPage not parseable");
        return;
    }

    LOG("Found doctype at: ", pos);

    pos += search_str.size();

    LOG("Doctype ends at:", pos);

    int start_addition = 0;
    int end_negation = 0;

    std::deque<SingleTag *> tag_stack;

    std::deque<Tag *> tag_tree;

    while (pos < this->html_content->size())
    {

        LOG("\n=========================================================");

        LOG("Current pos: ", pos);

        LOG("---------------------------------------------------------\n");

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
            LOG("Skipping to next tag");
            // pos++;
            // continue;
            pos = this->html_content->find('<', pos);
            if (pos == std::string::npos)
            {
                LOG("No more tags found");
                break;
            }
            LOG("Found start of tag at: ", pos);
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
        LOG("Found tag: ", tag);

        if (this->html_content->at(pos - 1) == '/')
        {
            tag_organisation = TagOrganisation::SELF_CLOSING;
            end_negation = 1;
        }
        else if (this->html_content->at(start_open + 1) == '/')
        {
            tag_organisation = TagOrganisation::CLOSING;
            start_addition = 1;
        }
        else
        {
            tag_organisation = TagOrganisation::OPENING;
        }


        // ###################################################################################
        //
        //          Find tag type

        next_space = this->html_content->find(' ', start_open); 

        if (next_space == std::string::npos || next_space > pos)
        {
            LOG("Using startclose");
            tag_type = this->html_content->substr(start_open + 1 + start_addition, pos - start_open - start_addition - 1 - end_negation);
        }
        else
        {
            LOG("Using next space");
            tag_type = this->html_content->substr(start_open + 1 + start_addition, next_space - start_open - 1 - start_addition);
        }

        LOG("Is ", (
                        tag_organisation == TagOrganisation::SELF_CLOSING ? "Self Closing" : tag_organisation == TagOrganisation::CLOSING ? "Closing"
                                                                                                                                            : "Opening"));

        LOG("Tag type: ", tag_type);


        // ###################################################################################
        //
        //          Push the found tag to the stack
        //
        // ###################################################################################

        if (tag_organisation == TagOrganisation::OPENING){
            LOG("Opening tag: ", tag_type);
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
            LOG("Self closing tag: ", tag_type);
            continue;
        }

        // ###################################################################################
        //
        //          Closing tags pop the open from the stack
        //
        // ###################################################################################

        LOG("Closing tag: ", tag_type);

        if (tag_stack.empty())
        {
            LOG("No opening tag to match closing tag: ", tag_type);
            continue;
        }

        SingleTag *sTag = tag_stack.front();

        // LOG("Top of stack: ", sTag->tag_type->c_str());
        // LOG("Tag ", sTag->tag->c_str());

        if (strcmp(tag_type.c_str(), sTag->tag_type->c_str()) != 0)
        {
            LOG("Closing tag: ", tag_type, " does not match opening tag: ", *sTag->tag_type);
            continue;
        }
        tag_stack.pop_front();


        LOG("Matched closing tag: ", tag_type, " with opening tag: ", *sTag->tag_type);

        Tag *tag = new Tag(*sTag->tag_type, *sTag->start_open, *sTag->start_close, start_open, pos);

        delete sTag;

        LOG("Created tag: ", tag->Name->c_str());
        
    }

    if (!tag_stack.empty())
    {
        LOG("Unclosed tags found");

        for (SingleTag *sTag : tag_stack)
        {
            LOG("Unclosed tag: ", sTag->tag_type->c_str());
            delete sTag;
        }
    }
    else
    {
        LOG("No unclosed tags found");
    }

    LOG("Finished scraping URL: ", this->url->c_str());
}