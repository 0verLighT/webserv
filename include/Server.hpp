#pragma once

#include <netinet/in.h>

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