#pragma once

#include "TOMLParserExecption.hpp"
#include <iostream>
#include <sstream>
#include <fstream>
#include <map>
#include "utils.hpp"

class TOMLParser
{
private:
	std::string _input;
	std::string _tmp_key;
	std::string _tmp_value;
	std::map<std::string, std::string>	_data;

public:
	TOMLParser();
	~TOMLParser();
	/*=============== GETTERS ===============*/

	std::string getKey(const std::string& line);
	std::string getValue(const std::string& line);

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

	/*=============== MEMBERS ===============*/

	void processInputFile(const std::string filepath);
	void printData(void);
};
