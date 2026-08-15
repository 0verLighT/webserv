#pragma once

#include "Client.hpp"
#include "Logger.hpp"
#include "RequestHandler.hpp"
#include "http/HttpRequest.hpp"
#include "utils.hpp"
#include <cerrno>
#include <csignal>
#include <cstddef>
#include <iostream>
#include <netinet/in.h>
#include <string.h>
#include <sys/select.h>
#include <unistd.h>

class Server {
  public:
    Server(int port);
    ~Server();
    void run();
  private:
    int _port;
    int _socket;
    sockaddr_in _serverAddress;
};