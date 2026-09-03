#pragma once

#include "TomlParserExecption.hpp"
#include "utils.hpp"
#include <iostream>
#include <sstream>
#include <fstream>
#include <map>
#include "utils.hpp"

template <typename T>
struct TomlValueConverter;

template <>
struct TomlValueConverter<std::string> {
  static std::string convert(const std::string& value) {
    return (value);
  }
};

template <>
struct TomlValueConverter<int> {
  static int convert(const std::string& value) {
    return (toInt(value));
  }
};

template <>
struct TomlValueConverter<float> {
  static float convert(const std::string& value) {
    return (toFloat(value));
  }
};

template <>
struct TomlValueConverter<bool> {
  static bool convert(const std::string& value) {
    return (toBool(value));
  }
};

class TomlParser {
  private:
    std::string _input;
    std::string _tmp_key;
    std::string _tmp_value;
    std::map<std::string, std::string>	_data;

  public:
    enum ValueType {
      STRING,
      INT,
      FLOAT,
      BOOL,
    };

    TomlParser();
    ~TomlParser();

    std::string	getKey(const std::string& line);
    std::string	getValue(const std::string& line);
    ValueType	getType(const std::string& var);

    template <typename T>
    T getValueFromKey(const std::string& key) {
      std::map<std::string, std::string>::iterator it = _data.find(key);
      if (it == _data.end())
        throw std::runtime_error("Value not found.");
      try {
        return TomlValueConverter<T>::convert(it->second);
      }
      catch (const std::exception& e) {
        throw;
      }
    }

    bool isValidLine(const std::string& line);
    bool isValidTable(const std::string& line);
    bool isValidPair(const std::string& line);
    bool isValidKey(const std::string& key);
    bool isValidValue(const std::string& value);
    bool isDuplicate(const std::string& key);

    void processInputFile(const std::string filepath);
    void printData(void);

    class InvalidFile: public TomlParserException {
      virtual const char *what(void) const throw();
    };

    class DuplicateKey: public TomlParserException {
      virtual const char *what(void) const throw();
    };

    class InvalidKey: public TomlParserException {
      virtual const char *what(void) const throw();
    };

    class InvalidValue: public TomlParserException {
      virtual const char *what(void) const throw();
    };
};
