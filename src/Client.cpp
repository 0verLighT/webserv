#include "Client.hpp"
#include "Logger.hpp"
#include <cerrno>
#include <cstddef>
#include <cstring>
#include <string>
#include <unistd.h>
#include "http/HttpRequest.hpp"
#include "utils.hpp"

Client::Client() : _socket(-1), _maxSizeReq(1), _maxBodySize(1),
  _remoteAddress("0.0.0.0"), _reqBuffer(""), _readToWrite(false),
  _requestTooLarge(false), _requestInvalid(false), _cgiPending(false),
  _cgiResponse(false), _cgiSucceeded(false) {}

Client::Client(int socket, const std::string& remoteAddress, std::size_t maxRequestSize,
               std::size_t maxBodySize) :
  _socket(socket), _maxSizeReq(maxRequestSize), _maxBodySize(maxBodySize),
  _remoteAddress(remoteAddress), _reqBuffer(""), _readToWrite(false),
  _requestTooLarge(false), _requestInvalid(false), _cgiPending(false),
  _cgiResponse(false), _cgiSucceeded(false) {}

int Client::getSocket() const {
  return _socket;
}

bool Client::readRequest() {
  char buffer[1024] = {0};
  if (_reqBuffer.size() >= _maxSizeReq) {
    _requestTooLarge = true;
    _readToWrite = true;
    return true;
  }
  std::size_t bytesToRead = sizeof(buffer);
  std::size_t remaining = _maxSizeReq - _reqBuffer.size();
  if (bytesToRead > remaining)
    bytesToRead = remaining;
  ssize_t bytesRead = recv(_socket, buffer, bytesToRead, 0);
  if (bytesRead == -1) {
    return true;
  } else if (bytesRead == 0) {
    return false;
  }
  _reqBuffer += std::string(buffer, bytesRead);

  size_t headerEnd  =_reqBuffer.find("\r\n\r\n");
  if (headerEnd != std::string::npos) {
    HttpRequest req;
    req.parseRequest(_reqBuffer);

    std::string contentLength = req.getHeader("content-length");
    if (!contentLength.empty()) {
      std::size_t length = 0;
      bool validLength = true;
      for (std::string::size_type i = 0; i < contentLength.size(); ++i) {
        if (contentLength[i] < '0' || contentLength[i] > '9') {
          validLength = false;
          break;
        }
        std::size_t digit = static_cast<std::size_t>(contentLength[i] - '0');
        if (length > (static_cast<std::size_t>(-1) - digit) / 10) {
          _requestTooLarge = true;
          _readToWrite = true;
          return true;
        }
        length = length * 10 + digit;
      }
      if (validLength && length > _maxBodySize) {
        _requestTooLarge = true;
        _readToWrite = true;
        return true;
      }
      if (!validLength) {
        _requestInvalid = true;
        _readToWrite = true;
        return true;
      }
    }

    if (req.isBodyComplete()) {
      if (req.getBody().size() > _maxBodySize) {
        _requestTooLarge = true;
        _readToWrite = true;
        return true;
      }
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

bool Client::isRequestTooLarge() const {
  return _requestTooLarge;
}

bool Client::isRequestInvalid() const {
  return _requestInvalid;
}

std::size_t Client::getMaxSizeReq() const {
  return _maxSizeReq;
}

bool Client::isCgiPending() const {
  return _cgiPending;
}

bool Client::hasCgiResponse() const {
  return _cgiResponse;
}

void Client::startCgi() {
  _cgi.startSubprocess();
  _cgiPending = true;
}

void Client::finishCgi(bool succeeded) {
  _cgiPending = false;
  _cgiResponse = true;
  _cgiSucceeded = succeeded;
  _readToWrite = true;
}

bool Client::cgiSucceeded() const {
  return _cgiSucceeded;
}

CommonGatewayInterface& Client::getCgi() {
  return _cgi;
}

int Client::closeConnection() {
  return close(_socket);
}

Client::~Client() {
  Logger::info("Client : " + to_string(_socket) + " was destroyed");
}
