cmake_minimum_required(VERSION 3.21)
project(WebCrawler)

set(BUILD_WITH_DEPENDENCIES ON) # disable if you want to manually include the dependencies (CURL, ZLIB, etc.) in your project

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Collect sources
file(GLOB SOURCES 
    "src/CurlManager.cpp"
    "src/WebPage.cpp"
    "src/Tags.cpp"
    "src/Utilities.cpp"
    "src/main.cpp"
)

# Include directories
include_directories(
    "src/inc/"
    "src/ext/incs/"
)

# Static library
set(BUILD_SHARED_LIBS OFF)
add_library(WebCrawler STATIC ${SOURCES})

if(NOT WIN32)
    find_package(CURL REQUIRED)
    target_link_libraries(WebCrawler PRIVATE CURL::libcurl stdc++)
    add_compile_options(-O3)

else()
    set(LIBS "${CMAKE_SOURCE_DIR}/src/ext/libs")
    message(STATUS "LIBS: ${LIBS}")

    # Add external library includes
    include_directories("${CMAKE_SOURCE_DIR}/src/ext/incs/curl")

    # Link external libraries properly
    target_link_libraries(WebCrawler PRIVATE 
        "${LIBS}/curl/libcurl.lib"
        "${LIBS}/zlib/zlib.lib"
        Ws2_32.lib
        Crypt32.lib
        Wldap32.lib
    )

    # Define macro for static linking
    target_compile_definitions(WebCrawler PRIVATE CURL_STATICLIB)

    # Optimization
    target_compile_definitions(WebCrawler PRIVATE /O2)
endif()


if(BUILD_WITH_DEPENDENCIES)
    if(WIN32)
        # Set library directory
        set(LIBS "${CMAKE_SOURCE_DIR}/src/ext/libs")

        # Define all static libraries
        set(WEB_CRAWLER_LIB ${CMAKE_BINARY_DIR}/Release/WebCrawler.lib)

        add_custom_command(
            TARGET WebCrawler POST_BUILD
            COMMAND lib.exe /OUT:${WEB_CRAWLER_LIB} 
                ${WEB_CRAWLER_LIB}
                "${LIBS}/curl/libcurl.lib"
                "${LIBS}/zlib/zlib.lib"
                Ws2_32.lib
                Crypt32.lib
                Wldap32.lib
        )

        message(STATUS "Merging WebCrawler with dependencies into WebCrawler.lib")
    endif()
endif()
