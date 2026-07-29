#pragma once

#include <iostream>
#include <sstream>

class TOMLParser
{
private:
	std::string _data;

public:
	/*=============== BUILDERS ===============*/

	// Default Constructor
	TOMLParser();
	// Destructor
	~TOMLParser();

	/*=============== GETTERS ===============*/

	std::string getKey();
	std::string getValue();

	/*=============== CHECKERS ===============*/

	bool isValidLine(std::string line);
	bool isValidTable(std::string line);
	bool isValidPair(std::string line);

	/*=============== CONVERTERS ===============*/

	/*=============== ERRORS ===============*/

	class TOMLParserException: public std::exception {};
	class InvalidFile: public TOMLParserException {
			virtual const char *what(void) const throw();
		};
};
