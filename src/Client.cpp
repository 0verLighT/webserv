#include "Client.hpp"
#include "Logger.hpp"
#include <cerrno>
#include <cstddef>
#include <cstring>
#include <string>
#include <unistd.h>
#include "http/HttpRequest.hpp"
#include "utils.hpp"

Client::Client() : _socket(-1), _reqBuffer(""), _readToWrite(false), _cgiPending(false), _cgiResponse(false) {}

Client::Client(int socket) : _socket(socket), _reqBuffer(""), _readToWrite(false), _cgiPending(false), _cgiResponse(false) {}

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

    if (req.isBodyComplete()) {
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

bool Client::isCgiPending() const { return _cgiPending; }
bool Client::hasCgiResponse() const { return _cgiResponse; }
void Client::startCgi() { _cgi.startSubprocess(); _cgiPending = true; }
void Client::finishCgi() { _cgiPending = false; _cgiResponse = true; _readToWrite = true; }
CommonGatewayInterface& Client::getCgi() { return _cgi; }

int Client::closeConnection() {
  return close(_socket);
}

Client::~Client() {
  Logger::info("Client : " + to_string(_socket) + " was destroyed");
}
