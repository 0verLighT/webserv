#include "Client.hpp"
#include "Logger.hpp"
#include <cerrno>
#include <cstring>
#include <unistd.h>

Client::Client(int socket) : _socket(socket) {
  char buffer[1024] = {0};
  ssize_t bytesRead = recv(socket, buffer, sizeof(buffer), 0);
  if (bytesRead == -1) {
    throw std::runtime_error("recv " + std::string(strerror(errno)));
  }
  _reqBuffer = std::string(buffer, bytesRead);
}

std::string Client::getReqBuffer() const {
  return _reqBuffer;
}

Client::~Client() {
  close(_socket);
  Logger::info("Client : " + Logger::to_string(_socket) + " was destroyed");
}