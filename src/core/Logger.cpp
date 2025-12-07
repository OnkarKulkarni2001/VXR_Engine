#include "Logger.h"

void Logger::Log(const std::string& message, LogLevel level) {
    switch (level) {
    case LogLevel::Info:  std::cout << "[INFO] " << message << "\n"; break;
    case LogLevel::Warn:  std::cout << "[WARN] " << message << "\n"; break;
    case LogLevel::Error: std::cerr << "[ERROR] " << message << "\n"; break;
    }
}
