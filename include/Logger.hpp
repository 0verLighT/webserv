#pragma once
#include <iostream>
#include <sstream>
#include <ctime>

#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define RESET "\033[0m"

class Logger {
public:
    Logger();
    ~Logger();
    static void info(const std::string& message);
    static void warn(const std::string& message);
    static void error(const std::string& message);
    template<typename T>
    static void debug(const T& message) {
      std::cout << getTimestamp() << BLUE << " DEBUG "<< std::endl << RESET << message << std::endl;
    }
    template<typename T>
    static std::string to_string(const T& value) {
      std::ostringstream oss;
      oss << value;
      return oss.str();
    }
private:
    static void log(const std::string& level, const std::string& message, const std::string& color);
    static std::string getTimestamp();
};
