#pragma once

#include "Client.hpp"
#include "Logger.hpp"
#include "enum/HttpStatus.hpp"
#include "http/HttpException.hpp"
#include "http/HttpRequest.hpp"
#include "http/HttpResponse.hpp"
#include "RequestHandler.hpp"
#include "utils.hpp"
#include <cerrno>
#include <csignal>
#include <cstddef>
#include <exception>
#include <iostream>
#include <map>
#include <netinet/in.h>
#include <poll.h>
#include <string.h>
#include <unistd.h>
#include <vector>

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