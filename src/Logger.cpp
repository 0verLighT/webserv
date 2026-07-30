#include "Logger.hpp"
#include <string>


Logger::Logger() {}

Logger::~Logger() {}

void Logger::log(const std::string &level, const std::string& message, const std::string& color) {
  if (message.empty())
    return;

  if (level == "ERROR") {
    std::cerr << getTimestamp() << " " << color << level << RESET << " " << message << std::endl;
  } else {
    std::cout << getTimestamp() << " " << color << level << RESET << " " << message << std::endl;
  }
}

std::string Logger::getTimestamp() {
  std::time_t	time = std::time(NULL);
  std::tm *ltm = std::localtime(&time);
  char buffer[32];
  std::strftime(buffer, sizeof(buffer), "[%Y/%m/%d/%H:%M:%S]", ltm);
  return std::string(buffer);
}

void Logger::info(const std::string& message) {
  log("INFO", message, GREEN);
}

void Logger::warn(const std::string& message) {
  log("WARN", message, YELLOW);
}

void Logger::error(const std::string& message) {
  log("ERROR", message, RED);
}
