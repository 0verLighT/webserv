#include "HttpRequest.hpp"
#include "Logger.hpp"
#include <iostream>
#include <iterator>
#include <string>
#include <sys/socket.h>
#include "enum/HttpMethod.hpp"


HttpRequest::HttpRequest() : _method(HttpMethod::UNKNOWN), _headers(), _body() {
  _methodMap["GET"] = HttpMethod::GET;
  _methodMap["POST"] = HttpMethod::POST;
  _methodMap["DELETE"] = HttpMethod::DELETE;
  _methodMap["PUT"] = HttpMethod::PUT;
  _methodMap["UNKNOWN"] = HttpMethod::UNKNOWN;
}

void HttpRequest::parseRequest(std::string buffer) {
  size_t pos = buffer.find("\r\n\r\n");
  if (pos != std::string::npos) {
    _body = buffer.substr(pos + 4);
    buffer = buffer.substr(0, pos + 2);
  }

  size_t headerEnd = buffer.find("\r\n");
  if (headerEnd != std::string::npos) {
    std::string fristLine = buffer.substr(0, headerEnd);

    _httpVersion = parseHttpVersion(fristLine);
    if (_httpVersion != "1.1") {
      // throw 505 error page
    }
    _method = parseMethod(fristLine);
    _path = parsePath(fristLine);
    _headers = parseHeaders(buffer.substr(headerEnd + 2));
  }
}

std::string HttpRequest::getHeader(std::string key) const {
  std::map<std::string, std::string>::const_iterator it = _headers.find(key);
  if (it != _headers.end()) {
    return it->second;
  }
  return "";
}

std::map<std::string, std::string> HttpRequest::parseHeaders(std::string req) const {
  std::map<std::string, std::string> headers;
  size_t pos = 0;
  while ((pos = req.find("\r\n")) != std::string::npos) {
    std::string line = req.substr(0, pos);
    size_t colon = line.find(":");
    if (colon != std::string::npos) {
      std::string key = line.substr(0, colon);
      std::string value = line.substr(colon + 1);
      size_t space = value.find_first_not_of(" \t\n\v\f\r");
      if (space != std::string::npos) {
        value = value.substr(space);
      }
      std::transform(key.begin(), key.end(), key.begin(), ::tolower);
      headers[key] = value;
    }
    req = req.substr(pos + 2);
  }
  return headers;
}

HttpMethod::Code HttpRequest::parseMethod(std::string req) const {
  size_t pos = req.find(" ");
  if (pos != std::string::npos) {
    req = req.substr(0, pos);
  }
  std::map<std::string, HttpMethod::Code>::const_iterator it = _methodMap.find(req);
  if (it != _methodMap.end()) {
    return it->second;
  }
  return HttpMethod::UNKNOWN;
}

std::string HttpRequest::parsePath(std::string req) const {
  size_t pos = req.find(" ");
  if (pos != std::string::npos) {
    std::string path = req.substr(pos + 1, req.find(" ", pos + 1) - pos - 1);
    std::string decoded;
    for (size_t i = 0; i < path.length(); ++i) {
      if (path[i] == '%') {
        std::string hex = path.substr(i + 1, 2);
        char c = static_cast<char>(std::strtol(hex.c_str(), NULL, 16));
        decoded += c;
        i += 2;
      } else {
        decoded += path[i];
      }
    }
    return decoded;
  }
  return "";
}

// return 1.1, 2.0
std::string HttpRequest::parseHttpVersion(std::string line) const {
  size_t pos = line.find_last_of("/");
  if (pos != std::string::npos) {
    std::string version = line.substr(pos + 1);
    return version;
  }
  return "";
}

HttpMethod::Code HttpRequest::getMethod() const {
  return _method;
}

std::string HttpRequest::getPath() const {
  return _path;
}

std::map<std::string, std::string> HttpRequest::getHeaders() const {
  return _headers;
}

std::string HttpRequest::getBody() const {
  return _body;
}

HttpRequest::~HttpRequest() {}