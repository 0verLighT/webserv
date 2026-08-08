#pragma once

#include "TOMLParserExecption.hpp"
#include <iostream>
#include <sstream>
#include <map>
#include "utils.hpp"

class TOMLParser
{
private:
	std::string _input;
	std::map<std::string, std::string>	_data;

public:
	TOMLParser();
	~TOMLParser();
	/*=============== GETTERS ===============*/

	// std::string getKey();
	// std::string getValue();

	/*=============== CHECKERS ===============*/

	bool isValidLine(const std::string& line);
	bool isValidTable(const std::string& line);
	bool isValidPair(const std::string& line);
	bool isValidKey(const std::string& key);
	bool isValidValue(const std::string& value);

	bool isString(const std::string& valueString);
	bool isMultilineString(const std::string& valueString);
	bool isLiteral(const std::string& valueString);
	bool isMultilineLiteral(const std::string& valueString);
	bool isInt(const std::string& valueString);
	bool isFloat(const std::string& valueString);
	bool isBool(const std::string& valueString);
	bool isBase(const std::string& valueString);

	/*=============== CONVERTERS ===============*/

	int		toInt(const std::string& valueString);
	float	toFloat(const std::string& valueString);
	bool	toBool(const std::string& valueString);

	/*=============== ERRORS ===============*/

	class InvalidFile: public TOMLParserException {
		virtual const char *what(void) const throw();
	};
};
