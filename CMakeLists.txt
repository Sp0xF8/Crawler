cmake_minimum_required(VERSION 3.21)
project(WebCrawler)

set(BUILD_WITH_DEPENDENCIES off) # disable if you want to manually include the dependencies (CURL, ZLIB, etc.) in your project, or if the project is being used as a submodule

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Define paths for include and lib
set(WEBCRAWLER_INCLUDE_DIR "${CMAKE_BINARY_DIR}/Crawler/include" CACHE PATH "WebCrawler Include Directory")
set(WEBCRAWLER_LIB_DIR "${CMAKE_BINARY_DIR}/Crawler/lib" CACHE PATH "WebCrawler Library Directory")

# Add the output directories for the library and headers
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${WEBCRAWLER_LIB_DIR})

# Collect sources
file(GLOB CRAWLER_SOURCES 
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
add_library(WebCrawler STATIC ${CRAWLER_SOURCES})
    
# Collect public headers
file(GLOB CRAWLER_HEADERS "src/inc/WebPage.hpp" "src/inc/Tags.hpp" "src/inc/Logger.hpp")

# Copy header files to output/include
add_custom_target(copy_headers ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/Crawler/include"
    COMMAND ${CMAKE_COMMAND} -E copy ${CRAWLER_HEADERS} "${CMAKE_BINARY_DIR}/Crawler/include"
    COMMENT "Copying header files to output/include"
)

add_dependencies(WebCrawler copy_headers)

if(NOT WIN32)
    find_package(CURL REQUIRED)
    target_link_libraries(WebCrawler PRIVATE CURL::libcurl stdc++)
    add_compile_options(-O3)

else()
    set(LIBS "${CMAKE_CURRENT_SOURCE_DIR}/src/ext/libs")
    message(STATUS "LIBS: ${LIBS}")

    # Add external library includes
    include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src/ext/incs/curl")

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

set(WEB_CRAWLER_LIB "")

if(BUILD_WITH_DEPENDENCIES)
    if(WIN32)
        # Set library directory
        set(LIBS "${CMAKE_CURRENT_SOURCE_DIR}/src/ext/libs")

        # Define all static libraries
        add_custom_command(
            TARGET WebCrawler POST_BUILD
            COMMAND lib.exe /OUT:${CMAKE_BINARY_DIR}/Crawler/lib/$<CONFIG>/WebCrawler.lib
                ${CMAKE_BINARY_DIR}/Crawler/lib/$<CONFIG>/WebCrawler.lib
                "${LIBS}/curl/libcurl.lib"
                "${LIBS}/zlib/zlib.lib"
                Ws2_32.lib
                Crypt32.lib
                Wldap32.lib
        )

        message(STATUS "Merging WebCrawler with dependencies into WebCrawler.lib")
    endif()
endif()

set_target_properties(WebCrawler PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${WEBCRAWLER_INCLUDE_DIR}"
    INTERFACE_LINK_DIRECTORIES "${WEBCRAWLER_LIB_DIR}"
)
