#include "Client.hpp"
#include <sys/socket.h>
#include <system_error>
#include <iostream>
#include <unistd.h>

Client::Client(int socket) : _socket(socket) {
  char buffer[1024] = {0};
  ssize_t bytesRead = recv(_socket, buffer, sizeof(buffer), 0);
  if (bytesRead == -1) {
    throw std::system_error(errno, std::generic_category());
  }
  std::cout << buffer << std::endl;
}

Client::~Client() {
  close(_socket);
  std::cout << "Client : " << _socket << "was destroyed" << std::endl;
}