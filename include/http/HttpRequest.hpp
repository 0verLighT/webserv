#pragma once

#include <string>
#include <map>
#include <cstdlib>
#include <algorithm>
#include "enum/HttpMethod.hpp"
#include "HttpException.hpp"
#include "enum/HttpStatus.hpp"

class HttpRequest {
  public:
    HttpRequest();
    ~HttpRequest();
    void parseRequest(std::string buffer);
    HttpMethod::Code getMethod() const;
    std::string getBody() const;
    std::map<std::string, std::string> getHeaders() const;
    std::string getHeader(std::string key) const;
    std::string getPath() const;
    std::map<std::string, std::string> parseHeaders(std::string req) const;
    HttpMethod::Code parseMethod(std::string req) const;
    std::string parsePath(std::string req) const;
    std::string parseHttpVersion(std::string req) const;
    void CheckHttpVersion(int socket);
    class HttpVersionNotSupported : public HttpException {
      public:
        HttpVersionNotSupported(int socket) : HttpException(HttpStatus::HTTP_VERSION_NOT_SUPPORTED, socket) {}
    };
  private:
    std::map<std::string, HttpMethod::Code> _methodMap;
    HttpMethod::Code _method;
    std::string _path;
    std::map<std::string, std::string> _headers;
    std::string _body;
    std::string _httpVersion;
};
