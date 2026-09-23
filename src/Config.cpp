#include "Config.hpp"

std::string Config::trim(const std::string& value) {
  std::string::size_type start = value.find_first_not_of(" \t\r\n");
  if (start == std::string::npos)
    return "";
  std::string::size_type end = value.find_last_not_of(" \t\r\n");
  return value.substr(start, end - start + 1);
}

std::string Config::stripQuotes(const std::string& value) {
  std::string trimmed = trim(value);
  if (trimmed.size() >= 2 && trimmed[0] == '"' && trimmed[trimmed.size() - 1] == '"')
    return trimmed.substr(1, trimmed.size() - 2);
  return trimmed;
}

std::vector<std::string> Config::splitList(const std::string& value) {
  std::vector<std::string> items;
  std::string cleaned = trim(value);
  if (cleaned.size() >= 2 && cleaned[0] == '[' && cleaned[cleaned.size() - 1] == ']')
    cleaned = cleaned.substr(1, cleaned.size() - 2);
  std::string current;
  for (std::string::size_type i = 0; i < cleaned.size(); ++i) {
    if (cleaned[i] == ',') {
      std::string item = trim(current);
      if (!item.empty())
        items.push_back(stripQuotes(item));
      current.clear();
      continue;
    }
    if (cleaned[i] == ' ' || cleaned[i] == '\t' || cleaned[i] == '\n' || cleaned[i] == '\r')
      continue;
    current += cleaned[i];
  }
  std::string item = trim(current);
  if (!item.empty())
    items.push_back(stripQuotes(item));
  return items;
}

Config::Config(const TomlParser& parser): _data(parser.getData()) {
  parseServerConfig();
  parseRouteConfig();
}

const Config::ServerConfig& Config::server() const {
  return _server;
}

const std::vector<Config::RouteConfig>& Config::routes() const {
  return _server.routes;
}

const Config::RouteConfig* Config::route(const std::string& path) const {
  for (std::vector<RouteConfig>::const_iterator it = _server.routes.begin();
       it != _server.routes.end(); ++it) {
    if (it->path == path)
      return &(*it);
  }
  return NULL;
}

void Config::parseServerConfig() {
  _server.autoindex = _data.find("server.autoindex") != _data.end() ? get<bool>("server.autoindex") : (_data.find("autoindex") != _data.end() ? get<bool>("autoindex") : true);
  _server.cgi_enabled = _data.find("server.cgi_enabled") != _data.end() ? get<bool>("server.cgi_enabled") : (_data.find("cgi_enabled") != _data.end() ? get<bool>("cgi_enabled") : true);
  _server.root = _data.find("server.root") != _data.end() ? stripQuotes(_data.find("server.root")->second) : (_data.find("root") != _data.end() ? stripQuotes(_data.find("root")->second) : "");
  _server.port = _data.find("server.port") != _data.end() ? get<int>("server.port") : (_data.find("port") != _data.end() ? get<int>("port") : 8080);
  _server.host = _data.find("server.host") != _data.end() ? get<std::string>("server.host") : (_data.find("host") != _data.end() ? get<std::string>("host") : "0.0.0.0");
  _server.server_name = _data.find("server.server_name") != _data.end() ? get<std::string>("server.server_name") : (_data.find("server_name") != _data.end() ? get<std::string>("server_name") : "localhost");
  _server.file = _data.find("server.file") != _data.end() ? get<std::string>("server.file") : (_data.find("file") != _data.end() ? get<std::string>("file") : "");
  _server.executor = _data.find("server.executor") != _data.end() ? get<std::string>("server.executor") : (_data.find("executor") != _data.end() ? get<std::string>("executor") : "");
  _server.timeout = _data.find("server.timeout") != _data.end() ? get<int>("server.timeout") : (_data.find("timeout") != _data.end() ? get<int>("timeout") : 5000);
  _server.default_error_page = _data.find("server.default_error_page") != _data.end() ? get<std::string>("server.default_error_page") : (_data.find("default_error_page") != _data.end() ? get<std::string>("default_error_page") : "");
  _server.max_body_size = _data.find("server.max_body_size") != _data.end() ? static_cast<std::size_t>(get<int>("server.max_body_size")) : (_data.find("max_body_size") != _data.end() ? static_cast<std::size_t>(get<int>("max_body_size")) : 1048576);

  if (_server.routes.empty()) {
    RouteConfig rootRoute;
    rootRoute.autoindex = _server.autoindex;
    rootRoute.directory_listing = true;
    rootRoute.cgi_enabled = _server.cgi_enabled;
    rootRoute.path = "/";
    rootRoute.root = _server.root.empty() ? "html" : _server.root;
    rootRoute.default_file = _server.file;
    rootRoute.upload_path = "";
    rootRoute.redirect = "";
    rootRoute.cgi_extension = "";
    rootRoute.default_error_page = _server.default_error_page;
    rootRoute.max_body_size = _server.max_body_size;
    rootRoute.methods.push_back("GET");
    rootRoute.methods.push_back("POST");
    rootRoute.methods.push_back("DELETE");
    _server.routes.push_back(rootRoute);
  }
}

void Config::parseRouteConfig() {
  std::map<std::string, std::string>::const_iterator rootIt = _data.find("server.route.root");
  std::map<std::string, std::string>::const_iterator methodsIt = _data.find("server.route.methods");
  std::map<std::string, std::string>::const_iterator autoindexIt = _data.find("server.route.autoindex");
  std::map<std::string, std::string>::const_iterator defaultFileIt = _data.find("server.route.default_file");
  std::map<std::string, std::string>::const_iterator uploadIt = _data.find("server.route.upload_path");
  std::map<std::string, std::string>::const_iterator redirectIt = _data.find("server.route.redirect");
  std::map<std::string, std::string>::const_iterator cgiEnabledIt = _data.find("server.route.cgi_enabled");
  std::map<std::string, std::string>::const_iterator cgiExtIt = _data.find("server.route.cgi_extension");
  std::map<std::string, std::string>::const_iterator errPageIt = _data.find("server.route.default_error_page");
  std::map<std::string, std::string>::const_iterator maxBodyIt = _data.find("server.route.max_body_size");

  if (rootIt == _data.end() && methodsIt == _data.end() && autoindexIt == _data.end() &&
      defaultFileIt == _data.end() && uploadIt == _data.end() && redirectIt == _data.end() &&
      cgiEnabledIt == _data.end() && cgiExtIt == _data.end() && errPageIt == _data.end() &&
      maxBodyIt == _data.end())
    return;

  RouteConfig route;
  route.path = "/";
  route.root = rootIt != _data.end() ? stripQuotes(rootIt->second) : (_server.root.empty() ? "html" : _server.root);
  route.autoindex = autoindexIt != _data.end() ? get<bool>("server.route.autoindex") : _server.autoindex;
  route.directory_listing = true;
  route.default_file = defaultFileIt != _data.end() ? stripQuotes(defaultFileIt->second) : _server.file;
  route.upload_path = uploadIt != _data.end() ? stripQuotes(uploadIt->second) : "";
  route.redirect = redirectIt != _data.end() ? stripQuotes(redirectIt->second) : "";
  route.methods = methodsIt != _data.end() ? splitList(methodsIt->second) : std::vector<std::string>();
  if (route.methods.empty()) {
    route.methods.push_back("GET");
    route.methods.push_back("POST");
    route.methods.push_back("DELETE");
  }
  route.cgi_enabled = cgiEnabledIt != _data.end() ? get<bool>("server.route.cgi_enabled") : _server.cgi_enabled;
  route.cgi_extension = cgiExtIt != _data.end() ? stripQuotes(cgiExtIt->second) : "";
  route.default_error_page = errPageIt != _data.end() ? stripQuotes(errPageIt->second) : _server.default_error_page;
  route.max_body_size = maxBodyIt != _data.end() ? static_cast<std::size_t>(get<int>("server.route.max_body_size")) : _server.max_body_size;
  _server.routes.push_back(route);
}

bool Config::has(const std::string& key) const {
  std::map<std::string, std::string>::const_iterator it = _data.find(key);
  if (it != _data.end())
    return true;
  std::string fallbackKey = key;
  std::string::size_type dotPos = fallbackKey.rfind('.');
  if (dotPos != std::string::npos)
    fallbackKey = fallbackKey.substr(dotPos + 1);
  for (std::map<std::string, std::string>::const_iterator it2 = _data.begin();
       it2 != _data.end(); ++it2) {
    std::string::size_type pos = it2->first.rfind('.');
    if (pos != std::string::npos && it2->first.substr(pos + 1) == fallbackKey)
      return true;
  }
  return false;
}
