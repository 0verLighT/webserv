#include "Server.hpp"
#include "utils.hpp"
#include "Client.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "RequestHandler.hpp"
#include "Logger.hpp"
#include <cerrno>
#include <csignal>
#include <cstddef>
#include <iostream>
#include <sys/select.h>
#include <unistd.h>
#include <string.h>
#include "enum/HttpStatus.hpp"

int eventLoop = 1;

void handlerSignal(int sig) {
  (void)sig;
  eventLoop = 0;
  Logger::info("Signal " + to_string(sig) + " received");
}

Server::Server(int port): _port(port) {
  Logger::info("Server Created");
  _serverAddress.sin_family =  AF_INET;
  _serverAddress.sin_port = htons(_port);
  _serverAddress.sin_addr.s_addr = INADDR_ANY;
  _socket = socket(AF_INET, SOCK_STREAM, 0);
  Logger::info("Socket Created at " + to_string(_port));
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
  Logger::info("Server Destroyed");
}

void Server::run() {
  signal(SIGINT, handlerSignal);
  // Catch Crtl + C signal
  while (eventLoop) {
    fd_set set;
    FD_ZERO(&set);
    FD_SET(_socket, &set);

    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    int activity = select(_socket + 1, &set, NULL, NULL, &timeout);
    if (activity == -1) {
      if (!eventLoop)
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
      HttpRequest req;

      req.parseRequest(client.getReqBuffer());
      RequestHandler handler(req, client.getSocket());
      handler.handleMethod();
      Logger::info("Cycle Event Loop");
    }
  }
}
