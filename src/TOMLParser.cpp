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
	// std::cout << "op = " << op <<std::endl;
	// size_t ed = line.find_last_of(']');
	size_t ed = line.find(']', op);
	// std::cout << "ed = " << ed <<std::endl;

	if (op == std::string::npos ||
		ed == std::string::npos ||
		line[ed + 1])
	{
		std::cout << "'" << line << "' is an invalid table." << std::endl;
		return (false);
	}

	std::cout << "'" << line << "' is a valid table." << std::endl;
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
		std::cout << "'" << line << "' is an invalid pair." << std::endl;
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
		// std::cout << "key = '" << key << "'" << std::endl;
		// std::cout << "value = '" << value << "'" << std::endl;
		std::cout << "'" << line << "' is an invalid pair." << std::endl;
		return (false);
	}

	// std::cout << "key = '" << key << "'" << std::endl;
	// std::cout << "value = '" << value << "'" << std::endl;
	std::cout << "'" << line << "' is a valid pair." << std::endl;
	return (true);
}

/*=============== CONVERTERS ===============*/

/*=============== ERRORS ===============*/

const char *TOMLParser::InvalidFile::what(void) const throw() {
	return ("Input TOML file is invalid.");
}
