#pragma once

// #define _DEBUG

#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>


class Logger {

    public:

        Logger(){


            std::string lastLogFile;
            //if the directory logs doesnt exist, make one

            if (!std::filesystem::exists("./logs")) {
                std::filesystem::create_directory("./logs");
            }

            int highest_index = -1;


            for (const auto& entry : std::filesystem::directory_iterator("./logs")) {
                std::string filename = entry.path().filename().string();
                if (filename.find("log-") == 0 && filename.find(".txt") == filename.size() - 4) {
                    int index = std::stoi(filename.substr(4, filename.size() - 4));
                    
                    if (index > highest_index) {
                        highest_index = index;
                    }
                }
            }


            logFile = "./logs/log-" + std::to_string(highest_index + 1) + ".txt";
        }
        ~Logger() {}

        template <typename ...Args>
        void log(Args... args) {
           (std::cout << ... << args) << std::endl;

            std::ofstream file(logFile, std::ios::app);
            if (file.is_open()) {
                ((file << args << " "), ...);
                file << std::endl;
                file.close();
            } else {
                std::cerr << "Failed to open log file" << std::endl;
            }
        }

        void close_file(){

        }


    private:

        std::string logFile;
        std::ofstream file;

};

extern Logger logger;


#ifdef _DEBUG
#define LOG(...) logger.log(__VA_ARGS__)
#else
#define LOG(...)
#endif