#include "Server.hpp"
#include "Client.hpp"
#include <iostream>
#include <unistd.h>

Server::Server(int port): _port(port) {
  this->_serverAddress.sin_family = AF_INET;
  this->_serverAddress.sin_port = htons(_port);
  this->_serverAddress.sin_addr.s_addr = INADDR_ANY;
  this->_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (_socket == -1) {
    throw std::runtime_error("socket");
  };
  if (bind(_socket, (struct sockaddr*)&_serverAddress, sizeof(_serverAddress)) == -1) {
    throw std::runtime_error("bind");
  };
  if (listen(_socket, 5) == -1) {
    throw std::runtime_error("listen");
  };
}

Server::~Server() {
  close(_socket);
  std::cout << "Server Dead" << std::endl;
}

void Server::run() {
  while (1) {
    int clientSocket = accept(_socket, NULL, NULL);
    if (clientSocket == -1) {
      std::cerr << "accept: " << strerror(errno) << std::endl;
      continue;
    };
    Client Client(clientSocket);
  }
}