#include "Client.hpp"
#include "HttpRequest.hpp"
#include <iostream>
#include <unistd.h>

Client::Client(int socket) : _socket(socket) {
  HttpRequest req;

  req.ReadRequest(_socket);
}

Client::~Client() {
  close(_socket);
  std::cout << "Client : " << _socket << " was destroyed" << std::endl;
}