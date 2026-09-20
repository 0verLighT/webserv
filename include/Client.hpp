#pragma once

#include <cstddef>
#include <string>
#include <sys/socket.h>
#include "CommonGatewayInterface.hpp"

class Client {
  public:
    Client();
    Client(int _socket, const std::string& remoteAddress = "0.0.0.0");
    ~Client();
    bool readRequest();
    int getSocket() const;
    bool getReadTowrite() const;
    bool isCgiPending() const;
    bool hasCgiResponse() const;
    CommonGatewayInterface& getCgi();
    void startCgi();
    void finishCgi(bool succeeded);
    bool cgiSucceeded() const;
    int closeConnection();
    std::string getReqBuffer() const;
    std::string getRemoteAddress() const;
  private:
    int _socket;
    std::string _remoteAddress;
    std::string _reqBuffer;
    bool _readToWrite;
    bool _cgiPending;
    bool _cgiResponse;
    bool _cgiSucceeded;
    CommonGatewayInterface _cgi;
};
