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

bool TomlParser::isValidLine(const std::string& line) {
  return (isValidPair(line) || isValidTable(line));
}

bool TomlParser::isValidTable(const std::string& line) {
  std::string trimmed = line;
  trimmed.erase(0, trimmed.find_first_not_of(" \t"));
  trimmed.erase(trimmed.find_last_not_of(" \t") + 1);

  size_t op = line.find_first_of('[');
  size_t ed = line.find(']', op);

  if (op == std::string::npos || ed == std::string::npos ||line[ed + 1])
    throw InvalidFile();
  return (true);
}

bool TomlParser::isValidPair(const std::string& line) {
  std::string trimmed = line;
  trimmed.erase(0, trimmed.find_first_not_of(" \t"));
  trimmed.erase(trimmed.find_last_not_of(" \t") + 1);

  std::string key;
  std::string value;
  size_t eqPos = trimmed.find('=');

  if (eqPos == std::string::npos)
    throw InvalidFile();

  key = trimmed.substr(0, eqPos);
  value = trimmed.substr(eqPos + 1);

  key.erase(0, key.find_first_not_of(" \t"));
  key.erase(key.find_last_not_of(" \t") + 1);
  value.erase(0, value.find_first_not_of(" \t"));
  value.erase(value.find_last_not_of(" \t") + 1);

  if (!(isValidKey(key) && isValidValue(value)))
    throw InvalidFile();

  _tmp_key = key;
  _tmp_value = trim(value, "\"");
  return (true);
}

bool TomlParser::isValidKey(const std::string& key) {
  if (isDuplicate(key))
    throw DuplicateKey();

  if (key.empty())
    throw InvalidKey();

  int qcount = 0;
  for (size_t i = 0; i < key.size(); i++)
  {
    if (key[i] == '"')
      qcount += 1;
  }
  if (qcount > 2 || qcount == 1)
    throw InvalidKey();

  return (true);
}

bool TomlParser::isValidValue(const std::string& value) {
  if (value.empty())
    throw InvalidValue();

  int qcount = 0;
  for (size_t i = 0; i < value.size(); i++)
  {
    if (value[i] == '"')
      qcount += 1;
  }
  if (qcount != 2 && (!(isBool(value) || isInt(value) || isFloat(value))))
    throw InvalidValue();

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
    if (isValidLine(line))
      _data[_tmp_key] = _tmp_value;
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
  return ("Invalid key format in input file.");
}

const char *TomlParser::InvalidValue::what(void) const throw() {
  return ("Invalid value format in input file.");
}


