#include "utils.hpp"
#include "Logger.hpp"
#include <cstddef>
#include <string>

const std::string getExtenstionFormPath(const std::string &path) {
  size_t dotPos = path.find_last_of('.');
  size_t sepPos = path.find_last_of("/\\");
  if (dotPos != std::string::npos && (sepPos == std::string::npos || dotPos > sepPos)) {
    std::string ext = path.substr(dotPos);
    Logger::debug(ext);
    return ext;
  }

  Logger::error("File has no extension");
  return "";
}
