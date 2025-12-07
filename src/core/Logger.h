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

// Logging convenience macros
#define LOG_INFO(msg) Logger::Log(msg, LogLevel::Info)
#define LOG_WARN(msg) Logger::Log(msg, LogLevel::Warn)
#define LOG_ERROR(msg) Logger::Log(msg, LogLevel::Error)
