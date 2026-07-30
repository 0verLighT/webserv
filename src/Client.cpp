#include "Client.hpp"
#include "HttpRequest.hpp"
#include "Logger.hpp"
#include <iostream>
#include <unistd.h>

Client::Client(int socket) : _socket(socket) {
  HttpRequest req;

  req.ReadRequest(_socket);
}

Client::~Client() {
  close(_socket);
  Logger::info("Client : " + Logger::to_string(_socket) + " was destroyed");
}