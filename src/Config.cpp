#include "Config.hpp"

Config::Config(const TomlParser& parser): _data(parser.getData()) {}

bool Config::has(const std::string& key) const {
  return _data.find(key) != _data.end();
}
