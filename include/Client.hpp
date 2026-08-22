#pragma once

#include <cstddef>
#include <string>
#include <sys/socket.h>

class Client {
  public:
    Client();
    Client(int _socket);
   ~Client();
   bool readRequest();
   int getSocket() const;
   bool getReadTowrite() const;
   int closeConnection();
   std::string getReqBuffer() const;
  private:
    int _socket;
    std::string _reqBuffer;
    bool _readToWrite;
};