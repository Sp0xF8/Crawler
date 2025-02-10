# Web Crawler Library - Sp0xF8

This library was entirely designed by me, depending only on libcurl and its subsequent dependancies. It is created to be a light-weight alternative to web scraping, allowing fast information gathering and parsing into `Markdown` format. It is also designed to be easily extendable, allowing for the addition of new features and functionality. 

The open source library has the primary goal of being a simple and easy to use tool for web scraping, with the added benefit of being able to parse the data into `Markdown` format. This allows for easy integration with other tools and services, with my own goal to make something which can be easily integrated into LLM usage. 

## Features
- **Lightweight**: The library is designed to be as light-weight as possible, with minimal dependancies and a small codebase.
- **Fast**: The library is designed to be as fast as possible, with minimal overhead and fast data retrieval.
- **Easy to Use**: The library is designed to be as easy to use as possible, with a simple API and easy to understand codebase.
- **Low Footprint**: The library is designed to have the lowest possible footprint, as LLMs are often limited in the number of tokens they can store. 


## Installation

Inside the CMakeLists.txt file, there is an option for building the library as static or dynamic. By default, the library is built to be a single import; meaning curl, zlib and other dependancies are baked into the final library. This is designed to make it as easy as possible to use the library, with minimal excess effort. 

In cases where this is not desireable, the post-build command can be easily toggled off with the `BUILD_WITH_DEPENDENCIES` flag. This will allow the library to be built without the dependancies, allowing for a smaller library size. This might also be preferable if the dependancies are already present on the system, or if the user wants to use a specific version of the dependancies. Given Curl is designed to be compatible with any version, with a consistent API, no issues should arise from mixing and matching versions to the developers needs.


## Usage

If you are building a project with cmake, you can simply add the repository as a `submodule`, and include the library in your project as a `subdirectory`. From there, you can simply link the library to your project, and include the header file in your source code. 

Alternatively, you can build the library as a standalone project, and link it to your project with just the files. The decision is up to you, and depends on your specific needs within the project. 

## Examples

The library is designed to be incredibly easy to use, with a simple API. Below are a few examples which highlight the simplicity of the library:

```cpp

#include "WebPage.hpp"

int main() {
    WebPage page("https://www.example.com"); // Create a new WebPage object with the URL

    page.scrape(); // Scrape the page from its html data

    page.write_markdown(); // Write the page to a markdown file (output.md) // Largely used for testing purposes in my use case

    std::cout << page.get_markdown() << std::endl; // Print the markdown data to the console


    return 0;
}

```
