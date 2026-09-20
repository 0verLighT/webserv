#pragma once

#include <cstddef>
#include <string>
#include <sys/socket.h>
#include "CommonGatewayInterface.hpp"

class Client {
  public:
    Client();
    Client(int _socket);
    ~Client();
    bool readRequest();
    int getSocket() const;
    bool getReadTowrite() const;
    bool isCgiPending() const;
    bool hasCgiResponse() const;
    CommonGatewayInterface& getCgi();
    void startCgi();
    void finishCgi();
    int closeConnection();
    std::string getReqBuffer() const;
  private:
    int _socket;
    std::string _reqBuffer;
    bool _readToWrite;
    bool _cgiPending;
    bool _cgiResponse;
    CommonGatewayInterface _cgi;
};
