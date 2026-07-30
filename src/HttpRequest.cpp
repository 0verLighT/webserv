#include "HttpRequest.hpp"
#include "Logger.hpp"
#include <iostream>
#include <sys/socket.h>


HttpRequest::HttpRequest() {}

std::string HttpRequest::ReadRequest(int socket) {
  char buffer[1024] = {0};
  ssize_t bytesRead = recv(socket, buffer, sizeof(buffer), 0);
  if (bytesRead == -1) {
    throw std::runtime_error("byte");
  }
  Logger::debug(buffer);
  return (std::string(buffer));
}

HttpRequest::~HttpRequest() {}