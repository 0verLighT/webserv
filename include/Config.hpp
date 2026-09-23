#pragma once

#include "TomlParser.hpp"
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

class Config {
  public:
    struct RouteConfig {
      bool autoindex;
      bool directory_listing;
      bool cgi_enabled;
      std::string path;
      std::string root;
      std::string default_file;
      std::string upload_path;
      std::string redirect;
      std::string cgi_extension;
      std::string default_error_page;
      std::size_t max_body_size;
      std::vector<std::string> methods;

      RouteConfig()
        : autoindex(true), directory_listing(true), cgi_enabled(true),
          path("/"), root(""), default_file(""), upload_path(""),
          redirect(""), cgi_extension(""), default_error_page(""),
          max_body_size(1048576), methods() {}
    };

    struct ServerConfig {
      int port;
      int timeout;
      bool cgi_enabled;
      bool autoindex;
      std::string host;
      std::string server_name;
      std::string file;
      std::string executor;
      std::string root;
      std::string default_error_page;
      std::size_t max_body_size;
      std::vector<RouteConfig> routes;

      ServerConfig()
        : port(8080), timeout(5000),
          cgi_enabled(true), autoindex(true),
          host("0.0.0.0"), server_name("localhost"), file(""),
          executor(""), root(""), default_error_page(""),
          max_body_size(1048576), routes() {}
    };

  private:
    std::map<std::string, std::string> _data;
    ServerConfig _server;

    void parseServerConfig();
    void parseRouteConfig();
    static std::string trim(const std::string& value);
    static std::string stripQuotes(const std::string& value);
    static std::vector<std::string> splitList(const std::string& value);

    /** @brief Reuses the parser's existing type validation.
    */
    template <typename T>
    T convertValue(const std::string& value) const {
      TomlParser parser;
      return parser.convertValue<T>(value);
    }

  public:
    /** @brief Copies all parsed values instead of retaining a reference to TomlParser.
    The explicit keyword prevents accidental implicit conversion and copy-construction.
    */
    explicit Config(const TomlParser& parser);

    const ServerConfig& server() const;
    const std::vector<RouteConfig>& routes() const;
    const RouteConfig* route(const std::string& path) const;

    /** @brief Looks up a key and convert its textual TOML value to the relevant type.
    Supported types are those implemented by TomlParser::convertValue.\
    */
    template <typename T>
    T get(const std::string& key) const {
      std::map<std::string, std::string>::const_iterator it = _data.find(key);
      if (it == _data.end()) {
        std::string fallbackKey = key;
        std::string::size_type dotPos = fallbackKey.rfind('.');
        if (dotPos != std::string::npos)
          fallbackKey = fallbackKey.substr(dotPos + 1);
        for (std::map<std::string, std::string>::const_iterator it2 = _data.begin();
             it2 != _data.end(); ++it2) {
          std::string::size_type pos = it2->first.rfind('.');
          if (pos != std::string::npos && it2->first.substr(pos + 1) == fallbackKey)
            return convertValue<T>(it2->second);
        }
        throw std::runtime_error("Value not found: " + key);
      }
      return convertValue<T>(it->second);
    }

    /** @brief Checks for optional settings without throwing an exception.
    */
    bool has(const std::string& key) const;
};
