#include "HttpRequest.hpp"
#include "Logger.hpp"
#include <iostream>
#include <sys/socket.h>


HttpRequest::HttpRequest() : _method(UNKNOWN), _headers(), _body() {
  _methodMap["GET"] = GET;
  _methodMap["POST"] = POST;
  _methodMap["DELETE"] = DELETE;
  _methodMap["PUT"] = PUT;
  _methodMap["UNKNOWN"] = UNKNOWN;
}

void HttpRequest::parseRequest(std::string buffer) {
  size_t pos = buffer.find("\r\n\r\n");
  if (pos != std::string::npos) {
    _body = buffer.substr(pos + 4);
    buffer = buffer.substr(0, pos);
  }

  size_t headerEnd = buffer.find("\r\n");
  if (headerEnd != std::string::npos) {
    std::string fristLine = buffer.substr(0, headerEnd);

    _method = parseMethod(fristLine);
    _path = parsePath(fristLine);
    _headers = parseHeaders(buffer.substr(headerEnd + 2));
  }
}

std::string HttpRequest::getHeader(std::string key) const {
  return _headers.at(key);
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
      headers[key] = value;
    }
    req = req.substr(pos + 2);
  }
  return headers;
}

HttpMethod HttpRequest::parseMethod(std::string req) const {
  size_t pos = req.find(" ");
  if (pos != std::string::npos) {
    req = req.substr(0, pos);
  }
  std::map<std::string, HttpMethod>::const_iterator it = _methodMap.find(req);
  if (it != _methodMap.end()) {
    return it->second;
  }
  return UNKNOWN;
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


HttpMethod HttpRequest::getMethod() const {
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