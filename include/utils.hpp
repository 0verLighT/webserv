#pragma once

#include <string>
#include <sstream>
#include <limits>
#include <stdexcept>

template<typename T>
static std::string to_string(const T& value) {
  std::ostringstream oss;
  oss << value;
  return oss.str();
}

const std::string getExtenstionFormPath(const std::string &path);
bool startsWith(const std::string& s, const std::string& prefix);
std::string trim(const std::string& s, const std::string& rm);
int anyBaseToInt(const std::string& number, int base);