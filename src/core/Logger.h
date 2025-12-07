#pragma once
#include <iostream>
#include <string>

enum class LogLevel {
    Info,
    Warn,
    Error
};

class Logger {
public:
    static void Log(const std::string& message, LogLevel level = LogLevel::Info);
};
