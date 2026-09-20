#include "Client.hpp"
#include "Logger.hpp"
#include <cerrno>
#include <cstddef>
#include <cstring>
#include <string>
#include <unistd.h>
#include "http/HttpRequest.hpp"
#include "utils.hpp"

Client::Client() : _socket(-1), _remoteAddress("0.0.0.0"), _reqBuffer(""), _readToWrite(false), _cgiPending(false), _cgiResponse(false), _cgiSucceeded(false) {}

Client::Client(int socket, const std::string& remoteAddress) :
  _socket(socket), _remoteAddress(remoteAddress), _reqBuffer(""),
  _readToWrite(false), _cgiPending(false), _cgiResponse(false), _cgiSucceeded(false) {}

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

std::string Client::getRemoteAddress() const {
  return _remoteAddress;
}

bool  Client::getReadTowrite() const {
  return _readToWrite;
}

bool Client::isCgiPending() const { return _cgiPending; }
bool Client::hasCgiResponse() const { return _cgiResponse; }
void Client::startCgi() { _cgi.startSubprocess(); _cgiPending = true; }
void Client::finishCgi(bool succeeded) {
  _cgiPending = false;
  _cgiResponse = true;
  _cgiSucceeded = succeeded;
  _readToWrite = true;
}
bool Client::cgiSucceeded() const { return _cgiSucceeded; }
CommonGatewayInterface& Client::getCgi() { return _cgi; }

int Client::closeConnection() {
  return close(_socket);
}

Client::~Client() {
  Logger::info("Client : " + to_string(_socket) + " was destroyed");
}
