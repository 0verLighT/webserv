#pragma once

#include <string>
#include <iostream>
#include <sstream>
#include <sys/socket.h>
#include <utility>
#include <vector>
#include "enum/HttpStatus.hpp"
#include "http/httpUtils.hpp"

class HttpResponse {
  public:
    HttpResponse(std::string _body, HttpStatus::Code _status, int _socket, std::string contentType);
    HttpResponse(std::string _body, HttpStatus::Code _status, int _socket, std::string contentType,
                 const std::vector<std::pair<std::string, std::string> >& extraHeaders);
    std::string serialize();
    void sendHttpResponse();
    ~HttpResponse();
  private:
    std::string _body;
    HttpStatus::Code _status;
    std::string _response;
    std::string _contentType;
    // CGI headers safe to forward after the server-owned response headers.
    std::vector<std::pair<std::string, std::string> > _extraHeaders;
    int _socket;
};
