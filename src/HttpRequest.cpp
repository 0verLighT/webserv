#include "HttpRequest.hpp"
#include <iostream>


int call = 0;

HttpRequest::HttpRequest() {}

std::string HttpRequest::ReadRequest(int socket) {
  char buffer[1024] = {0};
  ssize_t bytesRead = recv(socket, buffer, sizeof(buffer), 0);
  if (bytesRead == -1) {
    throw std::runtime_error("byte");
  }
  std::cout << buffer << std::endl;
  call++;

  std::cout << "call " << call << "\n";
  return (buffer);
}

HttpRequest::~HttpRequest() {}