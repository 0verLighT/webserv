#include "TOMLParser.hpp"

/*=============== BUILDERS ===============*/

TOMLParser::TOMLParser() {}

TOMLParser::~TOMLParser() {}

/*=============== GETTERS ===============*/

// std::string TOMLParser::getKey()
// {}

// std::string TOMLParser::getValue()
// {}

/*=============== CHECKERS ===============*/

bool TOMLParser::isValidLine(const std::string& line)
{
	return (isValidPair(line) || isValidTable(line));
}

bool TOMLParser::isValidTable(const std::string& line)
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
		std::cout << "X : `" << line << "` is an invalid table." << std::endl;
		return (false);
	}

	std::cout << "V : `" << line << "` is a valid table." << std::endl;
	return (true);
}

bool TOMLParser::isValidPair(const std::string& line)
{
	std::string trimmed = line;
	trimmed.erase(0, trimmed.find_first_not_of(" \t"));
	trimmed.erase(trimmed.find_last_not_of(" \t") + 1);

	std::string key;
	std::string value;
	size_t eqPos = trimmed.find('=');

	if (eqPos == std::string::npos)
	{
		std::cout << "X : `" << line << "` is an invalid pair (no equal sign)." << std::endl;
		return (false);
	}

	key = trimmed.substr(0, eqPos);
	value = trimmed.substr(eqPos + 1);

	key.erase(0, key.find_first_not_of(" \t"));
	key.erase(key.find_last_not_of(" \t") + 1);
	value.erase(0, value.find_first_not_of(" \t"));
	value.erase(value.find_last_not_of(" \t") + 1);

	if (!(isValidKey(key) && isValidValue(value)))
	{
		std::cout << "X : `" << line << "` is an invalid pair." << std::endl;
		return (false);
	}
	std::cout << "V : `" << line << "` is a valid pair." << std::endl;
	return (true);
}

// TODO
bool TOMLParser::isValidKey(const std::string& key)
{
	// std::cout << "DEBUG | Received key as `" << key << "`." << std::endl;
	if (key.empty())
	{
		// std::cout << "DEBUG | Key `" << key << "` is empty." << std::endl;
		return (false);
	}

	int qcount = 0;
	for (size_t i = 0; i < key.size(); i++)
	{
		if (key[i] == '"')
			qcount += 1;
	}
	if (qcount > 2)
	{
		// std::cout << "DEBUG | Key `" << key << "` contains too many quotes." << std::endl;
		return (false);
	}


	// size_t op = key.find_first_of('"');
	// size_t ed = key.find('"', op);

	return (true);
}

bool TOMLParser::isValidValue(const std::string& value)
{
	if (value.empty())
		return (false);

	int qcount = 0;
	for (size_t i = 0; i < value.size(); i++)
	{
		if (value[i] == '"')
			qcount += 1;
	}
	if (qcount != 2 && (!(isBool(value) || isInt(value) || isFloat(value))))
		return (false);

	return (true);
}

// bool TOMLParser::isString(const std::string& valueString)
// {}

// bool TOMLParser::isMultilineString(const std::string& valueString)
// {}

// bool TOMLParser::isLiteral(const std::string& valueString)
// {}

// bool TOMLParser::isMultilineLiteral(const std::string& valueString)
// {}

bool TOMLParser::isInt(const std::string& valueString)
{
	std::string trimmed = trim(valueString, "_");
	int value;
	std::istringstream iss(trimmed);
	return (iss >> value) && iss.eof();
}

bool TOMLParser::isFloat(const std::string& valueString)
{
	std::string trimmed = trim(valueString, "_");
	float value;
	std::istringstream iss(trimmed);
	return (iss >> value) && iss.eof();
}

bool TOMLParser::isBool(const std::string& valueString)
{
	return (valueString == "true" || valueString == "false");
}

bool TOMLParser::isBase(const std::string& valueString)
{
	return (startsWith(valueString, "0b") ||
			startsWith(valueString, "0o") ||
			startsWith(valueString, "0x"));
}

/*=============== CONVERTERS ===============*/

int TOMLParser::toInt(const std::string& valueString)
{
	int value;
	std::istringstream iss(valueString);
	iss >> value;
	return (value);
}

float TOMLParser::toFloat(const std::string& valueString)
{
	float value;
	std::istringstream iss(valueString);
	iss >> value;
	return (value);
}

bool TOMLParser::toBool(const std::string& valueString)
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
