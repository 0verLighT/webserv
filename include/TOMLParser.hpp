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
	enum ValueType {
		STRING,
		INT,
		FLOAT,
		BOOL,
	};

	TOMLParser();
	~TOMLParser();

	std::string	getKey(const std::string& line);
	std::string	getValue(const std::string& line);
	ValueType	getType(const std::string& var);

	template <typename T>
	T getValueFromKey(const std::string& key) {
		std::map<std::string, std::string>::iterator it = _data.find(key);
		if (it == _data.end())
			throw std::runtime_error("Value not found.");
		try {
			return convertValue<T>(it->second);
		}
		catch (const std::exception& e) {
			throw;
		}
	}

	/*=============== CHECKERS ===============*/

	bool isValidLine(const std::string& line);
	bool isValidTable(const std::string& line);
	bool isValidPair(const std::string& line);
	bool isValidKey(const std::string& key);
	bool isValidValue(const std::string& value);

	bool isDuplicate(const std::string& key);

	/*=============== CONVERTERS ===============*/

	template <typename T>
	T convertValue(const std::string& value) {
		switch (getType(value))
		{
		case INT:
			return (toInt(value));
			break;
		case FLOAT:
			return (toFloat(value));
			break;
		case BOOL:
			return (toBool(value));
			break;
		case STRING:
			return (value);
			break;
		default:
			throw std::runtime_error("Impossible value conversion.");
			break;
		}
	}

	/*=============== MEMBERS ===============*/

	void processInputFile(const std::string filepath);
	void printData(void);

	/*=============== ERRORS ===============*/

	class InvalidFile: public TOMLParserException {
		virtual const char *what(void) const throw();
	};

	class DuplicateKey: public TOMLParserException {
		virtual const char *what(void) const throw();
	};

	class InvalidKey: public TOMLParserException {
		virtual const char *what(void) const throw();
	};

	class InvalidValue: public TOMLParserException {
		virtual const char *what(void) const throw();
	};
};
