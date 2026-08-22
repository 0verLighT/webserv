#include "Client.hpp"
#include "Logger.hpp"
#include <cerrno>
#include <cstddef>
#include <cstring>
#include <string>
#include <unistd.h>
#include "http/HttpRequest.hpp"
#include "utils.hpp"

Client::Client() : _socket(-1), _reqBuffer(""), _readToWrite(false) {}

Client::Client(int socket) : _socket(socket), _reqBuffer(""), _readToWrite(false) {}

int Client::getSocket() const {
  return _socket;
}

bool Client::readRequest() {
  char buffer[1024] = {0};
  ssize_t bytesRead = recv(_socket, buffer, sizeof(buffer), 0);
  if (bytesRead == -1) {
    return true;
  }
  if (bytesRead == 0) {
    return false;
  }
  _reqBuffer += std::string(buffer, bytesRead);

  size_t headerEnd  =_reqBuffer.find("\r\n\r\n");
  if (headerEnd != std::string::npos) {
    HttpRequest req;
    req.parseRequest(_reqBuffer);

    std::string res = req.getHeader("content-length");
    size_t bodyContent = 0;
    if (!res.empty())
      bodyContent = (size_t)toInt(res);
    size_t bodySize = _reqBuffer.size() - (headerEnd + 4);

    if (bodySize >= bodyContent) {
      _readToWrite = true;
    }
  }
  return true;
}

std::string Client::getReqBuffer() const {
  return _reqBuffer;
}

bool  Client::getReadTowrite() const {
  return _readToWrite;
}


int Client::closeConnection() {
  return close(_socket);
}

Client::~Client() {
  Logger::info("Client : " + to_string(_socket) + " was destroyed");
}