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

bool endsWith(const std::string& s, const std::string& suffix)
{
	size_t pos = s.rfind(suffix);
	if (pos == s.size() - suffix.size())
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

/*=============== NUMBER UTILS ===============*/

int anyBaseToInt(const std::string& number, int base)
{
	int res = 0;
	std::size_t i = 0;
	bool neg = false;

	if (!number.empty() && (number[0] == '+' || number[0] == '-'))
	{
		neg = (number[0] == '-');
		i = 1;
	}

	if (i == number.size())
		throw std::invalid_argument("Invalid number.");

	for (; i < number.size(); ++i)
	{
		char c = std::toupper(static_cast<unsigned char>(number[i]));
		int digit;

		if (c >= '0' && c <= '9')
			digit = c - '0';
		else if (c >= 'A' && c <= 'Z')
			digit = c - 'A' + 10;
		else
			throw std::invalid_argument("Invalid character in number.");
		if (digit >= base)
			throw std::invalid_argument("Digit out of range for base.");
		if (res > (std::numeric_limits<int>::max() - digit) / base)
			throw std::overflow_error("Integer overflow.");
		res = res * base + digit;
	}
	return (neg ? -res : res);
}
