#pragma once
#include <cstddef>
#include <sys/socket.h>

class Client {
  public:
    Client(int _socket);
   ~Client();
   void ReadRequest();
  private:
    int _socket;
};