#include "TOMLParser.hpp"

/*=============== BUILDERS ===============*/

TOMLParser::TOMLParser() {}

TOMLParser::~TOMLParser() {}

/*=============== GETTERS ===============*/

//TODO
std::string TOMLParser::getKey(const std::string& line)
{
	return (line);
}

// TODO
std::string TOMLParser::getValue(const std::string& line)
{
	return (line);
}

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
		// std::cout << "X : `" << line << "`" << std::endl;
		return (false);
	}

	// std::cout << "V : `" << line << "`" << std::endl;
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
		// std::cout << "X : `" << line << "` (no equal sign)." << std::endl;
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
		// std::cout << "X : `" << line << "`"  << std::endl;
		_tmp_key = "";
		_tmp_value = "";
		return (false);
	}
	// std::cout << "V : `" << line << "`" << std::endl;
	_tmp_key = key;
	_tmp_value = value;
	return (true);
}

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

bool TOMLParser::isFloat(const std::string& valueString)
{
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

/*=============== MEMBERS ===============*/

// TODO: already assigned key protection
void TOMLParser::processInputFile(const std::string filepath)
{
	std::ifstream file(filepath.c_str());
	if (!file)
	{
		throw std::runtime_error("Couldn't open input file.");
	}
	std::string line;
	while (std::getline(file, line))
	{
		if (startsWith(line, "#") || line.empty())
			continue;
		if (isValidLine(line))
			_data[_tmp_key] = _tmp_value;
	}
	file.close();
}

void TOMLParser::printData(void)
{
	for (std::map<std::string, std::string>::iterator it = _data.begin(); it != _data.end(); ++it)
	{
		std::cout << it->first << " = " << it->second << std::endl;
	}
}
