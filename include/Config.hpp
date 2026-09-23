#pragma once

#include "TomlParser.hpp"
#include <map>
#include <stdexcept>
#include <string>

class Config {
  private:
    // A copy of the parser's data.
    std::map<std::string, std::string> _data;

  public:
    /** @brief Copies all parsed values instead of retaining a reference to TomlParser.
    The explicit keyword prevents accidental implicit conversion and copy-construction.
    */
    explicit Config(const TomlParser& parser);

    /** @brief Looks up a key and convert its textual TOML value to the relevant type.
    Supported types are those implemented by TomlParser::convertValue.\
    */
    template <typename T>
    T get(const std::string& key) const {
      std::map<std::string, std::string>::const_iterator it = _data.find(key);
      if (it == _data.end()) {
        for (std::map<std::string, std::string>::const_iterator it2 = _data.begin();
             it2 != _data.end(); ++it2) {
          std::string::size_type dotPos = it2->first.rfind('.');
          if (dotPos != std::string::npos && it2->first.substr(dotPos + 1) == key)
            return convertValue<T>(it2->second);
        }
        throw std::runtime_error("Value not found: " + key);
      }
      return convertValue<T>(it->second);
    }

    /** @brief Checks for optional settings without throwing an exception.
    */
    bool has(const std::string& key) const;

  private:
    /** @brief Reuses the parser's existing type validation.
    */
    template <typename T>
    T convertValue(const std::string& value) const {
      TomlParser parser;
      return parser.convertValue<T>(value);
    }
};
