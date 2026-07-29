#include "Server.hpp"
#include "Client.hpp"
#include <cerrno>
#include <csignal>
#include <cstddef>
#include <iostream>
#include <sys/select.h>
#include <unistd.h>
#include <string.h>

int EventLoop = 1;

//! TODO : implement logger when signal are trigger
void handlerSignal(int sig) {
  (void)sig;
  EventLoop = 0;
}

Server::Server(int port): _port(port) {
  memset(&_serverAddress, 0, sizeof(_serverAddress));
  this->_serverAddress.sin_family = AF_INET;
  this->_serverAddress.sin_port = htons(_port);
  this->_serverAddress.sin_addr.s_addr = INADDR_ANY;
  this->_socket = socket(AF_INET, SOCK_STREAM, 0);
  int opt = 1;
  if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    throw std::runtime_error("setsockopt: " + std::string(strerror(errno)));
  }
  if (_socket == -1) {
    throw std::runtime_error("socket: " + std::string(strerror(errno)));
  };
  if (bind(_socket, (struct sockaddr*)&_serverAddress, sizeof(_serverAddress)) == -1) {
    throw std::runtime_error("bind: " + std::string(strerror(errno)));
  };
  if (listen(_socket, 5) == -1) {
    throw std::runtime_error("listen: " + std::string(strerror(errno)));
  };
}

Server::~Server() {
  close(_socket);
  std::cout << "Server Dead" << std::endl;
}

void Server::run() {
  signal(SIGINT, handlerSignal);
  // Catch Crtl + C signal
  while (EventLoop) {
    fd_set set;
    FD_ZERO(&set);
    FD_SET(_socket, &set);

    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    int activity = select(_socket + 1, &set, NULL, NULL, &timeout);
    if (activity == -1) {
      if (!EventLoop)
        break;
      std::cerr << "select" << std::endl;
      continue;
    }
    if (activity == 0)
      continue;
    if (FD_ISSET(_socket, &set)) {
      int clientSocket = accept(_socket, NULL, NULL);
      if (clientSocket == -1) {
        std::cerr << "accept: " << strerror(errno) << std::endl;
        continue;
      };
      Client client(clientSocket);
    }
  }
}
