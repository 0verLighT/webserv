#pragma once
#include <cstddef>
#include <string>
#include <sys/socket.h>

class Client {
  public:
    Client(int _socket);
   ~Client();
   int getSocket() const;
   std::string getReqBuffer() const;
  private:
    int _socket;
    std::string _reqBuffer;
};