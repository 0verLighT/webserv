#include "Client.hpp"
#include "Logger.hpp"
#include <cerrno>
#include <cstring>
#include <unistd.h>
#include "utils.hpp"

Client::Client() : _socket(-1), _reqBuffer("") {}

Client::Client(int socket) : _socket(socket), _reqBuffer("") {}

int Client::getSocket() const {
  return _socket;
}

void Client::readRequest() {
  char buffer[1024] = {0};
  ssize_t bytesRead = recv(_socket, buffer, sizeof(buffer), 0);
  if (bytesRead == -1) {
    throw std::runtime_error("recv " + std::string(strerror(errno)));
  }
  _reqBuffer = std::string(buffer, bytesRead);
}

std::string Client::getReqBuffer() const {
  return _reqBuffer;
}

void Client::closeConnection() {
  close(_socket);
}

Client::~Client() {
  Logger::info("Client : " + to_string(_socket) + " was destroyed");
}