#pragma once
#include <string>
#include <iostream>
#include <sstream>
#include <sys/socket.h>
#include "enum/ContentType.hpp"
#include "enum/HttpStatus.hpp"

class HttpResponse {
  public:
    HttpResponse(std::string _body, HttpStatus::Code _status, int _socket);
    std::string serialize();
    void sendHttpResponse();
    std::string getSentenceResponseHttpStatus(HttpStatus::Code status) const;
    ~HttpResponse();
  private:
    std::string _body;
    HttpStatus::Code _status;
    std::string _response;
    int _socket;
};
