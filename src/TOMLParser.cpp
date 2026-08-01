#include "TOMLParser.hpp"
#include <cctype>

/*=============== BUILDERS ===============*/

TOMLParser::TOMLParser() {}

TOMLParser::~TOMLParser() {}

/*=============== GETTERS ===============*/

// std::string TOMLParser::getKey()
// {}

// std::string TOMLParser::getValue()
// {}

/*=============== CHECKERS ===============*/

bool TOMLParser::isValidLine(std::string line)
{
	return (isValidPair(line) || isValidTable(line));
}

bool TOMLParser::isValidTable(std::string line)
{
	std::string trimmed = line;
	trimmed.erase(0, trimmed.find_first_not_of(" \t"));
	trimmed.erase(trimmed.find_last_not_of(" \t") + 1);

	size_t op = line.find_first_of('[');
	size_t ed = line.find(']', op);

	if (op == std::string::npos ||
		ed == std::string::npos ||
		line[ed + 1])
	{
		std::cout << "`" << line << "` is an invalid table." << std::endl;
		return (false);
	}

	std::cout << "`" << line << "` is a valid table." << std::endl;
	return (true);
}

bool TOMLParser::isValidPair(std::string line)
{
	std::string trimmed = line;
	trimmed.erase(0, trimmed.find_first_not_of(" \t"));
	trimmed.erase(trimmed.find_last_not_of(" \t") + 1);

	std::string key;
	std::string value;
	size_t eqPos = trimmed.find('=');

	if (eqPos == std::string::npos)
	{
		std::cout << "`" << line << "` is an invalid pair." << std::endl;
		return (false);
	}

	key = trimmed.substr(0, eqPos);
	value = trimmed.substr(eqPos + 1);

	key.erase(0, key.find_first_not_of(" \t"));
	key.erase(key.find_last_not_of(" \t") + 1);
	value.erase(0, value.find_first_not_of(" \t"));
	value.erase(value.find_last_not_of(" \t") + 1);

	if (key.empty() || value.empty() || value.size() < 2 || value[0] != '"' || value[value.size() - 1] != '"')
	{
		std::cout << "`" << line << "` is an invalid pair." << std::endl;
		return (false);
	}
	std::cout << "`" << line << "` is a valid pair." << std::endl;
	return (true);
}

bool TOMLParser::isValidKey(std::string key)
{
	(void)key;
	return (true);
}

bool TOMLParser::isValidValue(std::string value)
{
	(void)value;
	return (true);
}

/**
int5 = 1_000
int6 = 5_349_221
int7 = 53_49_221
int8 = 1_2_3_4_5

# hexadecimal with prefix `0x`
hex1 = 0xDEADBEEF
hex2 = 0xdeadbeef
hex3 = 0xdead_beef

# octal with prefix `0o`
oct1 = 0o01234567
oct2 = 0o755

# binary with prefix `0b`
bin1 = 0b11010110
 */
bool TOMLParser::isInt(std::string valueString)
{
	int value;
	std::istringstream iss(valueString);
	return (iss >> value) && iss.eof();
}

bool TOMLParser::isFloat(std::string valueString)
{
	float value;
	std::istringstream iss(valueString);
	return (iss >> value) && iss.eof();
}

bool TOMLParser::isBool(std::string valueString)
{
	return (valueString == "true" || valueString == "false");
}

/*=============== CONVERTERS ===============*/

int TOMLParser::toInt(std::string valueString)
{
	int value;
	std::istringstream iss(valueString);
	iss >> value;
	return (value);
}

float TOMLParser::toFloat(std::string valueString)
{
	float value;
	std::istringstream iss(valueString);
	iss >> value;
	return (value);
}

bool TOMLParser::toBool(std::string valueString)
{
	if (valueString == "true")
		return (true);
	else
		return (false);
}

/*=============== ERRORS ===============*/

const char *TOMLParser::InvalidFile::what(void) const throw() {
	return ("Input TOML file is invalid.");
}
