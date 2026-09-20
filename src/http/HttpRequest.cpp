#include "http/HttpRequest.hpp"
#include "Logger.hpp"
#include "utils.hpp"
#include <cerrno>
#include <cstddef>
#include <cctype>
#include <fcntl.h>
#include <filesystem>
#include <iostream>
#include <iterator>
#include <map>
#include <string>
#include <sys/socket.h>
#include "enum/HttpMethod.hpp"

static bool hasChunkedEncoding(const std::string& value) {
  std::string lower(value);
  for (std::string::size_type i = 0; i < lower.size(); ++i)
    lower[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(lower[i])));
  return lower.find("chunked") != std::string::npos;
}

// Returns false for an incomplete body and true once the terminating chunk is present.
static bool decodeChunkedBody(const std::string& input, std::string& body) {
  std::string::size_type offset = 0;
  body.clear();
  while (true) {
    std::string::size_type lineEnd = input.find("\r\n", offset);
    if (lineEnd == std::string::npos)
      return false;
    std::string sizeText = input.substr(offset, lineEnd - offset);
    std::string::size_type extension = sizeText.find(';');
    if (extension != std::string::npos)
      sizeText.erase(extension);
    std::string::size_type first = sizeText.find_first_not_of(" \t");
    std::string::size_type last = sizeText.find_last_not_of(" \t");
    if (first == std::string::npos)
      return false;
    sizeText = sizeText.substr(first, last - first + 1);
    if (sizeText.empty())
      return false;

    size_t chunkSize = 0;
    for (std::string::size_type i = 0; i < sizeText.size(); ++i) {
      unsigned int digit;
      if (sizeText[i] >= '0' && sizeText[i] <= '9')
        digit = static_cast<unsigned int>(sizeText[i] - '0');
      else if (sizeText[i] >= 'a' && sizeText[i] <= 'f')
        digit = static_cast<unsigned int>(sizeText[i] - 'a' + 10);
      else if (sizeText[i] >= 'A' && sizeText[i] <= 'F')
        digit = static_cast<unsigned int>(sizeText[i] - 'A' + 10);
      else
        return false;
      if (chunkSize > (static_cast<size_t>(-1) - digit) / 16)
        return false;
      chunkSize = chunkSize * 16 + digit;
    }

    offset = lineEnd + 2;
    if (chunkSize == 0) {
      if (input.compare(offset, 2, "\r\n") == 0)
        return true;
      return input.find("\r\n\r\n", offset) != std::string::npos;
    }
    if (chunkSize > static_cast<size_t>(-1) - 2 ||
      input.size() - offset < chunkSize + 2)
      return false;
    body.append(input, offset, chunkSize);
    offset += chunkSize;
    if (input.compare(offset, 2, "\r\n") != 0)
      return false;
    offset += 2;
  }
}


HttpRequest::HttpRequest() : _method(HttpMethod::UNKNOWN), _headers(), _body(), _bodyComplete(false), _queryString() {
  _methodMap["GET"] = HttpMethod::GET;
  _methodMap["POST"] = HttpMethod::POST;
  _methodMap["DELETE"] = HttpMethod::DELETE;
  _methodMap["PUT"] = HttpMethod::PUT;
  _methodMap["UNKNOWN"] = HttpMethod::UNKNOWN;
}

void HttpRequest::parseRequest(std::string buffer) {
  _bodyComplete = false;
  size_t pos = buffer.find("\r\n\r\n");
  if (pos != std::string::npos) {
    _body = buffer.substr(pos + 4);
    buffer = buffer.substr(0, pos + 2);
  }

  size_t headerEnd = buffer.find("\r\n");
  if (headerEnd != std::string::npos) {
    std::string firstLine = buffer.substr(0, headerEnd);
    // Logger::info(firstLine);

    _httpVersion = parseHttpVersion(firstLine);
    _method = parseMethod(firstLine);
    _path = parsePathWithQueries(firstLine);
    _headers = parseHeaders(buffer.substr(headerEnd + 2));
    if (hasChunkedEncoding(getHeader("transfer-encoding"))) {
      std::string decodedBody;
      _bodyComplete = decodeChunkedBody(_body, decodedBody);
      if (_bodyComplete)
        _body = decodedBody;
    } else {
      std::string contentLength = getHeader("content-length");
      if (contentLength.empty()) {
        _bodyComplete = true;
      } else {
        size_t expected = static_cast<size_t>(toInt(contentLength));
        _bodyComplete = _body.size() >= expected;
        if (_body.size() > expected)
          _body.resize(expected);
      }
    }
  }
}

void  HttpRequest::CheckHttpVersion(int socket) {
  if (_httpVersion != "1.1") {
    throw HttpVersionNotSupported(socket);
  }
}

std::string HttpRequest::getHeader(std::string key) const {
  std::map<std::string, std::string>::const_iterator it = _headers.find(key);
  if (it != _headers.end()) {
    return it->second;
  }
  return "";
}

std::string HttpRequest::getQuery(std::string key) const {
  std::map<std::string, std::string>::const_iterator it = _queries.find(key);
  if (it != _queries.end()) {
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
      // Logger::info(key);
      // Logger::info(value);
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

std::string HttpRequest::parsePathWithQueries(std::string req) {
  size_t pos = req.find(" ");
  std::string path;
  if (pos != std::string::npos) {
    std::string pathWithQueries = req.substr(pos + 1, req.find(" ", pos + 1) - pos - 1);
    std::string decoded;
    for (size_t i = 0; i < pathWithQueries.length(); ++i) {
      if (pathWithQueries[i] == '%') {
        std::string hex = pathWithQueries.substr(i + 1, 2);
        char c = static_cast<char>(std::strtol(hex.c_str(), NULL, 16));
        decoded += c;
        i += 2;
      } else {
        decoded += pathWithQueries[i];
      }
    }
    size_t hasQueries = decoded.find("?");
    if (hasQueries != std::string::npos) {
    //   Logger::info("Query detected");
      path = decoded.substr(0, hasQueries);
      _queryString = pathWithQueries.substr(pathWithQueries.find("?") + 1);
      _queries = parseQueries(decoded, hasQueries);
      }
      return path.empty() ? decoded : path;
  }
  return "";
}


std::map<std::string, std::string> HttpRequest::parseQueries(std::string decoded, size_t hasQueries) const {
  std::map<std::string, std::string>queriesMap;

  size_t start = 0;
  std::string queries = decoded.substr(hasQueries + 1);
  while (start <= queries.length()) {
    size_t separator = queries.find("&", start);

    if (separator == std::string::npos) {
      separator = queries.length();
    }

    std::string keyAndValue = queries.substr(start, separator - start);
    size_t eqPos = keyAndValue.find("=");
    if (eqPos == std::string::npos) {
      std::string key = keyAndValue.substr(0, eqPos);
      std::string value = keyAndValue.substr(eqPos + 1);
      queriesMap[key] = value;
      Logger::info("key : " + key + " Value : " + value);
    } else if (!keyAndValue.empty()) {
      queriesMap[keyAndValue] = "";
    }
    start = separator + 1;
  }
  return queriesMap;
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

std::string HttpRequest::getQueryString() const {
  return _queryString;
}

std::string HttpRequest::getHttpVersion() const {
  return _httpVersion;
}

std::map<std::string, std::string> HttpRequest::getHeaders() const {
  return _headers;
}

std::string HttpRequest::getBody() const {
  return _body;
}

bool HttpRequest::isBodyComplete() const {
  return _bodyComplete;
}

HttpRequest::~HttpRequest() {}
