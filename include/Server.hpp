#pragma once

#include "Client.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "Logger.hpp"
#include "RequestHandler.hpp"
#include "enum/HttpStatus.hpp"
#include "utils.hpp"

#include <cerrno>
#include <csignal>
#include <cstddef>
#include <iostream>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>

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