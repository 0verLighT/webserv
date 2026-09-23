#pragma once

#include "TomlParserException.hpp"
#include "utils.hpp"
#include <iostream>
#include <sstream>
#include <fstream>
#include <map>

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
      return convertValue<T>(it->second);
    }

    bool isValidLine(const std::string& line);
    bool isValidTable(const std::string& line);
    bool isValidPair(const std::string& line);
    bool isValidKey(const std::string& key);
    bool isValidValue(const std::string& value);
    bool isDuplicate(const std::string& key);

    template <typename T>
    T convertValue(const std::string& value) {
      return convertValue(value, static_cast<T*>(0));
    }
    template <typename T>
    T convertValue(const std::string& value, T*) {
      (void)value;
      throw std::runtime_error("Unsupported conversion type.");
    }
    int convertValue(const std::string& value, int*);
    float convertValue(const std::string& value, float*);
    bool convertValue(const std::string& value, bool*);
    std::string convertValue(const std::string& value, std::string*);

    void processInputFile(const std::string filepath);
    void printData(void);

    const std::map<std::string, std::string>& getData(void) const;

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
