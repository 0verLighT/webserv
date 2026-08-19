#pragma once

#include <cstddef>
#include <string>
#include <sys/socket.h>

class Client {
  public:
    Client();
    Client(int _socket);
   ~Client();
   void readRequest();
   int getSocket() const;
   void closeConnection();
   std::string getReqBuffer() const;
  private:
    int _socket;
    std::string _reqBuffer;
};