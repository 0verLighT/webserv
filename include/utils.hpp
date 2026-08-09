#pragma once

#include <string>
#include <sstream>
#include <stdexcept>
#include <cctype>
#include <iostream>
#include <limits>
#include <cmath>

bool	isInt(const std::string& valueString);
bool	isFloat(const std::string& valueString);
bool	isBool(const std::string& valueString);
bool	isBase(const std::string& valueString);

int		toInt(const std::string& valueString);
float	toFloat(const std::string& valueString);
bool	toBool(const std::string& valueString);

template<typename T>
static std::string to_string(const T& value) {
  std::ostringstream oss;
  oss << value;
  return oss.str();
}

const std::string getExtenstionFormPath(const std::string &path);
bool startsWith(const std::string& s, const std::string& prefix);
bool endsWith(const std::string& s, const std::string& suffix);
std::string trim(const std::string& s, const std::string& rm);
int anyBaseToInt(const std::string& number, int base);
