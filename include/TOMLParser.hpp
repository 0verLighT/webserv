#pragma once

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

	bool isValidLine(std::string line);
	bool isValidTable(std::string line);
	bool isValidPair(std::string line);
	bool isValidKey(std::string key);
	bool isValidValue(std::string value);

	bool isInt(std::string valueString);
	bool isFloat(std::string valueString);
	bool isBool(std::string valueString);
	bool isBase(std::string valueString);

	/*=============== CONVERTERS ===============*/

	int		toInt(std::string valueString);
	float	toFloat(std::string valueString);
	bool	toBool(std::string valueString);

	/*=============== ERRORS ===============*/

	class TOMLParserException: public std::exception {};
	class InvalidFile: public TOMLParserException {
		virtual const char *what(void) const throw();
	};
};
