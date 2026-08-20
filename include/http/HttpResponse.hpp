#pragma once

#include <string>
#include <iostream>
#include <sstream>
#include <sys/socket.h>
#include "enum/HttpStatus.hpp"
#include "http/httpUtils.hpp"

class HttpResponse {
  public:
    HttpResponse(std::string _body, HttpStatus::Code _status, int _socket, std::string contentType);
    std::string serialize();
    void sendHttpResponse();
    ~HttpResponse();
  private:
    std::string _body;
    HttpStatus::Code _status;
    std::string _response;
    std::string _contentType;
    int _socket;
};
