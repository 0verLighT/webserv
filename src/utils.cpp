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

bool startsWith(const std::string& s, const std::string& prefix)
{
	size_t pos = s.find(prefix);
	if (pos == 0)
		return (true);
	return (false);
}

std::string trim(const std::string& s, const std::string& rm)
{
	std::string trimmed = s;
	size_t pos = 0;

	while ((pos = trimmed.find(rm, pos)) != std::string::npos)
		trimmed.erase(pos, rm.size());
	return (trimmed);
}
