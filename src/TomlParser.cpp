#include "TomlParser.hpp"

TomlParser::TomlParser() {}

TomlParser::~TomlParser() {}

TomlParser::ValueType TomlParser::getType(const std::string& var) {
  if (isInt(var))
    return (INT);
  else if (isFloat(var))
    return (FLOAT);
  else if (isBool(var))
    return (BOOL);
  else
    return (STRING);
}

const std::map<std::string, std::string>& TomlParser::getData(void) const {
  return _data;
}

int TomlParser::convertValue(const std::string& value, int*) {
  if (getType(value) != INT)
    throw std::runtime_error("Value is not an integer.");
  return toInt(value);
}

float TomlParser::convertValue(const std::string& value, float*) {
  if (getType(value) != FLOAT)
    throw std::runtime_error("Value is not a float.");
  return toFloat(value);
}

bool TomlParser::convertValue(const std::string& value, bool*) {
  if (getType(value) != BOOL)
    throw std::runtime_error("Value is not a boolean.");
  return toBool(value);
}

std::string TomlParser::convertValue(const std::string& value, std::string*) {
  if (getType(value) != STRING)
    throw std::runtime_error("Value is not a string.");

  std::string trimmed = trim(value, " \t\r\n");
  if (trimmed.size() >= 2 && trimmed[0] == '"' && trimmed[trimmed.size() - 1] == '"')
    return trimmed.substr(1, trimmed.size() - 2);
  return trimmed;
}

bool TomlParser::isValidLine(const std::string& line) {
  return (isValidPair(line) || isValidTable(line));
}

bool TomlParser::isValidTable(const std::string& line) {
  std::string trimmed = line;
  trimmed.erase(0, trimmed.find_first_not_of(" \t"));
  trimmed.erase(trimmed.find_last_not_of(" \t") + 1);

  if (trimmed.empty() || trimmed[0] != '[')
    return false;

  bool isArrayTable = false;
  if (trimmed.size() >= 2 && trimmed[0] == '[' && trimmed[1] == '[') {
    isArrayTable = true;
    if (trimmed.size() < 4 || trimmed[trimmed.size() - 2] != ']' || trimmed[trimmed.size() - 1] != ']')
      return false;
    trimmed = trimmed.substr(2, trimmed.size() - 4);
  } else {
    size_t closing = trimmed.find(']');
    if (closing == std::string::npos || closing + 1 != trimmed.size())
      return false;
    trimmed = trimmed.substr(1, closing - 1);
  }

  std::string tableName = trim(trimmed, " \t");
  if (tableName.empty())
    return false;

  if (isArrayTable) {
    int index = _array_table_counters[tableName];
    std::ostringstream oss;
    oss << index;
    _current_table = tableName + "." + oss.str();
    _array_table_counters[tableName] = index + 1;
  } else {
    _current_table = tableName;
  }
  _tmp_key = _current_table;
  _tmp_value = "";
  return true;
}

bool TomlParser::isValidPair(const std::string& line) {
  std::string trimmed = line;
  trimmed.erase(0, trimmed.find_first_not_of(" \t"));
  trimmed.erase(trimmed.find_last_not_of(" \t") + 1);

  if (trimmed.empty() || trimmed[0] == '[')
    return false;

  std::string key;
  std::string value;
  size_t eqPos = trimmed.find('=');

  if (eqPos == std::string::npos)
    return (false);

  key = trimmed.substr(0, eqPos);
  value = trimmed.substr(eqPos + 1);

  key.erase(0, key.find_first_not_of(" \t"));
  key.erase(key.find_last_not_of(" \t") + 1);
  value.erase(0, value.find_first_not_of(" \t"));
  value.erase(value.find_last_not_of(" \t") + 1);

  if (!(isValidKey(key) && isValidValue(value)))
    return (false);

  _tmp_key = _current_table.empty() ? key : _current_table + "." + key;
  _tmp_value = value;
  return (true);
}

bool TomlParser::isValidKey(const std::string& key) {
  if (isDuplicate(key))
    return (false);

  if (key.empty())
    return (false);

  int qcount = 0;
  for (size_t i = 0; i < key.size(); i++)
  {
    if (key[i] == '"')
      qcount += 1;
  }
  if (qcount > 2 || qcount == 1)
    return (false);

  return (true);
}

bool TomlParser::isValidValue(const std::string& value) {
  if (value.empty())
    return (false);

  if (value[0] == '[' && value[value.size() - 1] == ']')
    return true;

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

bool TomlParser::isDuplicate(const std::string& key) {
  return (_data.find(key) != _data.end());
}

void TomlParser::processInputFile(const std::string filepath) {
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
    if (isValidTable(line))
      continue;
    if (isValidPair(line))
      _data[_tmp_key] = _tmp_value;
    else
      throw InvalidFile();
  }
  file.close();
}

void TomlParser::printData(void) {
  for (std::map<std::string, std::string>::iterator it = _data.begin(); it != _data.end(); ++it)
  {
    std::cout << it->first << " = " << it->second << std::endl;
  }
}

const char *TomlParser::InvalidFile::what(void) const throw() {
  return ("Input TOML file is invalid.");
}

const char *TomlParser::DuplicateKey::what(void) const throw() {
  return ("Duplicate key in input file.");
}

const char *TomlParser::InvalidKey::what(void) const throw() {
  return ("Invalid key format.");
}

const char *TomlParser::InvalidValue::what(void) const throw() {
  return ("Invalid value format.");
}
