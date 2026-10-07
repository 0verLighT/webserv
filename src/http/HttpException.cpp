#include "http/HttpException.hpp"
#include "Logger.hpp"
#include "enum/HttpStatus.hpp"
#include "http/HttpResponse.hpp"
#include "http/httpUtils.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>

HttpException::HttpException(HttpStatus::Code code, int socket) : _code(code), _socket(socket) {
  _messageCode = getSentenceResponseHttpStatus(_code);
}

void HttpException::SendExceptionResponse(const std::string& errorPage) const {
  std::string content;
  if (_code >= HttpStatus::BAD_REQUEST && !errorPage.empty()) {
    std::ifstream customFile(errorPage.c_str());
    if (customFile.is_open()) {
      std::stringstream fileContent;
      fileContent << customFile.rdbuf();
      content = fileContent.str();
    } else {
      Logger::error("Configured error page could not be opened: " + errorPage);
    }
  }

  if (content.empty() && _code >= HttpStatus::BAD_REQUEST) {
    std::ifstream templateFile("httpError/template/template.html");
    if (!templateFile.is_open()) {
      Logger::error("Template html error doesn't exist");
      return;
    }
    std::stringstream fileContent;
    fileContent << templateFile.rdbuf();
    content = fileContent.str();
  }

  replaceAll(content, "{ERROR}", getSentenceResponseHttpStatus(_code));
  HttpResponse res(content, _code, _socket, "text/html");
  res.sendHttpResponse();
}

const char *HttpException::what() const throw() {
  return _messageCode.c_str();
}

HttpStatus::Code HttpException::statusCode() const {
  return _code;
}

HttpException::~HttpException() throw() {}
