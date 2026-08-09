#include "utils.hpp"
#include "Logger.hpp"

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

bool	isInt(const std::string& valueString)
{
	std::string trimmed = trim(valueString, "_");
	int value;

	if (isBase(trimmed))
	{
		try
		{
			if (startsWith(trimmed, "0b"))
				value = anyBaseToInt(&trimmed[2], 2);
			if (startsWith(trimmed, "0o"))
				value = anyBaseToInt(&trimmed[2], 8);
			if (startsWith(trimmed, "0x"))
				value = anyBaseToInt(&trimmed[2], 16);
			return (true);
		}
		catch(const std::exception& e)
		{
			return (false);
		}
	}

	std::istringstream iss(trimmed);
	return (iss >> value) && iss.eof();
}

bool	isFloat(const std::string& valueString)
{
	if (startsWith(valueString, ".") || endsWith(valueString, "."))
		return (false);

	if (valueString.find(".e") != std::string::npos)
		return (false);

	if (valueString == "inf" ||
		valueString == "+inf" ||
		valueString == "-inf" ||
		valueString == "nan" ||
		valueString == "+nan" ||
		valueString == "-nan")
		return (true);

	std::string trimmed = trim(valueString, "_");
	float value;
	std::istringstream iss(trimmed);
	return (iss >> value) && iss.eof();
}

bool	isBool(const std::string& valueString)
{
	return (valueString == "true" || valueString == "false");
}

bool	isBase(const std::string& valueString)
{
	return (startsWith(valueString, "0b") ||
			startsWith(valueString, "0o") ||
			startsWith(valueString, "0x"));
}

/*=============== CONVERTERS ===============*/

int		toInt(const std::string& valueString)
{
	int value;
	std::istringstream iss(valueString);
	iss >> value;
	return (value);
}

float	toFloat(const std::string& valueString)
{
	if (valueString == "inf" ||
		valueString == "+inf")
		return (std::numeric_limits<float>::infinity());
	if (valueString == "-inf")
		return (-std::numeric_limits<float>::infinity());
	if (valueString == "nan" ||
		valueString == "+nan" ||
		valueString == "-nan")
		return (nanf(valueString.c_str()));

	float value;
	std::istringstream iss(valueString);
	iss >> value;
	return (value);
}

bool	toBool(const std::string& valueString)
{
	if (valueString == "true")
		return (true);
	else
		return (false);
}

bool	startsWith(const std::string& s, const std::string& prefix)
{
	size_t pos = s.find(prefix);
	if (pos == 0)
		return (true);
	return (false);
}

bool	endsWith(const std::string& s, const std::string& suffix)
{
	size_t pos = s.rfind(suffix);
	if (pos == s.size() - suffix.size())
		return (true);
	return (false);
}

std::string	trim(const std::string& s, const std::string& rm)
{
	std::string trimmed = s;
	size_t pos = 0;

	while ((pos = trimmed.find(rm, pos)) != std::string::npos)
		trimmed.erase(pos, rm.size());
	return (trimmed);
}

/*=============== NUMBER UTILS ===============*/

int	anyBaseToInt(const std::string& number, int base)
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
