#pragma once
#include <string>
#include <iostream>
#include <sstream>
#include <sys/socket.h>

enum HttpStatus {
  OK = 200,
  NOT_FOUND = 404,
  INTERNAL_SERVER_ERROR = 500,
};

namespace ContentType {
  const std::string TEXT_PLAIN = "text/plain";
  const std::string TEXT_HTML = "text/html";
  const std::string APPLICATION_JSON = "application/json";
  const std::string APPLICATION_XML = "application/xml";
  const std::string APPLICATION_OCTET_STREAM = "application/octet-stream";
  const std::string APPLICATION_FORM_URLENCODED = "application/x-www-form-urlencoded";
  const std::string APPLICATION_MULTIPART_FORM_DATA = "multipart/form-data";
}

class HttpResponse {
  public:
    HttpResponse(std::string _body, HttpStatus _status, int _socket);
    std::string serialize();
    void sendHttpResponse();
    ~HttpResponse();
  private:
    std::string _body;
    HttpStatus _status;
    std::string _response;
    int _socket;
};
