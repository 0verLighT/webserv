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

void HttpException::SendExecptionResponse() const {
  std::string templatePath = "httpError/template/template.html";
  std::ifstream file(templatePath.c_str());
  if (!file.is_open()) {
    Logger::error("Template html error doesn't exit");
    // Change this with throws
    return ;
  }
  std::stringstream fileContent;
  fileContent << file.rdbuf();
  std::string content = fileContent.str();

  replaceAll(content, "{ERROR}", getSentenceResponseHttpStatus(_code));
  Logger::debug(content);
  HttpResponse res(content, _code, _socket, "text/html");
}

const char *HttpException::what() const throw() {
  return _messageCode.c_str();
}

HttpException::~HttpException() throw() {}
