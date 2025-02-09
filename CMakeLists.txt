cmake_minimum_required(VERSION 3.21)
project(WebCrawler)

# set cxx version to 17

set(CMAKE_CXX_STANDARD 17)


file(GLOB SOURCES 
    "src/*.cpp"
)

include_directories(
    "src/inc/"
    "src/ext/incs/"
)

add_executable(WebCrawler ${SOURCES})



if(NOT WIN32)

    find_package(CURL REQUIRED)
    target_link_libraries(WebCrawler PRIVATE CURL::libcurl)

    target_link_libraries(WebCrawler PRIVATE stdc++) # add libcurl.a here
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(/O2)
    endif()
else()

    set(LIBS "${CMAKE_SOURCE_DIR}/src/ext/libs")
    message(STATUS "LIBS: ${LIBS}")

    target_link_libraries(WebCrawler PRIVATE 
        "${LIBS}/curl/libcurl.lib"
        "${LIBS}/zlib/zlib.lib"
        Ws2_32.lib
        Crypt32.lib
        Wldap32.lib
        )

    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(-O3)
    endif()
endif()

