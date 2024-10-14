#pragma once

#include <iostream>
#include <ctime>
#include <iomanip>

namespace ntn
{

#undef ERROR

enum class LogLevel 
{
    INFO,
    WARNING,
    ERROR,
};

class Log 
{
public:
    Log() = default;

    static void info(const std::string& message) 
    {
        log(LogLevel::INFO, message);
    }

    static void warning(const std::string& message) 
    {
        log(LogLevel::WARNING, message);
    }

    static void error(const std::string& message) 
    {
        log(LogLevel::ERROR, message);
    }

private:
    static void log(LogLevel level, const std::string& message) 
    {
        std::time_t now = std::time(nullptr);
        std::tm tm_info;
        localtime_s(&tm_info, &now);
        char timestamp[20];
        strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", &tm_info);

        std::cout << "[" << timestamp << "] ";

        switch (level) 
        {
        case LogLevel::INFO:
            std::cout << "[INFO] ";
            break;
        case LogLevel::WARNING:
            std::cout << "[WARNING] ";
            break;
        case LogLevel::ERROR:
            std::cout << "[ERROR] ";
            break;
        }

        std::cout << message << std::endl;
    }
};
}