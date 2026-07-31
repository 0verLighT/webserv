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

void HttpRequest::ReadRequest(int socket) {
  char buffer[1024] = {0};
  ssize_t bytesRead = recv(socket, buffer, sizeof(buffer), 0);
  if (bytesRead == -1) {
    throw std::runtime_error("byte");
  }
  std::string request(buffer);
  size_t pos = request.find("\r\n\r\n");
  if (pos != std::string::npos) {
    _body = request.substr(pos + 4);
    request = request.substr(0, pos);
  }

  size_t headerEnd = request.find("\r\n");
  if (headerEnd != std::string::npos) {
    std::string FristLine = request.substr(0, headerEnd);

    _method = ParseMethod(FristLine);
    _path = ParsePath(FristLine);
    _headers = ParseHeaders(request.substr(headerEnd + 2));
  }
}

std::string HttpRequest::GetHeader(std::string key) const {
  return _headers.at(key);
}

std::map<std::string, std::string> HttpRequest::ParseHeaders(std::string req) const {
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

HttpMethod HttpRequest::ParseMethod(std::string req) const {
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

std::string HttpRequest::ParsePath(std::string req) const {
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


HttpMethod HttpRequest::GetMethod() const {
  return _method;
}

std::string HttpRequest::GetPath() const {
  return _path;
}

std::map<std::string, std::string> HttpRequest::GetHeaders() const {
  return _headers;
}

std::string HttpRequest::GetBody() const {
  return _body;
}

HttpRequest::~HttpRequest() {}