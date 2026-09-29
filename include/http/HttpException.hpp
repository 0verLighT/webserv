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
    HttpStatus::Code statusCode() const;
    virtual void SendExceptionResponse(const std::string& errorPage = "") const;

    virtual ~HttpException() throw();
};
