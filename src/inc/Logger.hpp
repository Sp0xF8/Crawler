#pragma once

// #define _DEBUG

#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <windows.h>


class Logger {

    public:

        Logger(){
            this->hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

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
        void warn(Args... args) {
            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
            std::cerr << "[WARNING] ";
            SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
            (std::cerr << ... << args) << std::endl;
            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

            std::ofstream file(logFile, std::ios::app);
            if (file.is_open()) {
                ((file << args << " "), ...);
                file << std::endl;
                file.close();
            } else {
                std::cerr << "Failed to open log file" << std::endl;
            }
        }



        template <typename ...Args>
        void error_log(Args... args) {
            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
            std::cerr << "[ERROR] ";
            (std::cerr << ... << args) << std::endl;
            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

            std::ofstream file(logFile, std::ios::app);
            if (file.is_open()) {
                ((file << args << " "), ...);
                file << std::endl;
                file.close();
            } else {
                std::cerr << "Failed to open log file" << std::endl;
            }
        }



        template <typename ...Args>
        void log(Args... args) {
            SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
            std::cout << "[INFO] ";
            SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
            (std::cout << ... << args) << std::endl;
            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

            std::ofstream file(logFile, std::ios::app);
            if (file.is_open()) {
                ((file << args << " "), ...);
                file << std::endl;
                file.close();
            } else {
                std::cerr << "Failed to open log file" << std::endl;
            }
        }

        template <typename ...Args>
        void wlog(Args... args) {
            SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
            std::wcout.imbue(std::locale("en_US.utf8"));
            (std::wcout << ... << args) << std::endl;
            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);


            std::wofstream file(logFile, std::ios::app);
            if (file.is_open()) {
                ((file << args << " "), ...);
                file << std::endl;
                file.close();
            } else {
                std::wcerr << "Failed to open log file" << std::endl;
            }
        }

        void close_file(){

        }


    private:

        std::string logFile;
        std::ofstream file;
        HANDLE hConsole;

};

extern Logger logger;


#ifdef _DEBUG
    #define LOG(...)    logger.log(__VA_ARGS__)
    #define WLOG(...)   logger.wlog(__VA_ARGS__)
    #define WARN(...)   logger.warn(__VA_ARGS__)
    #define ERROR_LOG(...)  logger.error_log(__VA_ARGS__)
#else
    #define LOG(...)
    #define WLOG(...)
    #define WARN(...)
    #define ERROR_LOG(...)
#endif
