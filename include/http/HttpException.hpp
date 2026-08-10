#pragma once

#include "enum/HttpStatus.hpp"
#include "exception"
#include "http/HttpResponse.hpp"
#include <exception>
#include <string>

class HttpException: public std::exception {
  protected:
    HttpStatus::Code _code;
    std::string _messageCode;
    int _socket;

  public:
    HttpException(HttpStatus::Code code, int socket);

    virtual const char* what() const throw();
    virtual void SendExecptionResponse() const;

    virtual ~HttpException() throw();
};
