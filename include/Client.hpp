#pragma once

#include <cstddef>
#include <string>
#include <sys/socket.h>
#include "CommonGatewayInterface.hpp"

class Client {
  public:
    Client();
    Client(int _socket, const std::string& remoteAddress = "0.0.0.0",
      std::size_t maxRequestSize = 1024, std::size_t maxBodySize = 1024);
    ~Client();
    bool readRequest();
    int getSocket() const;
    std::size_t getMaxSizeReq() const;
    bool getReadTowrite() const;
    bool isRequestTooLarge() const;
    bool isRequestInvalid() const;
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
    std::size_t _maxSizeReq;
    std::size_t _maxBodySize;
    std::string _remoteAddress;
    std::string _reqBuffer;
    bool _readToWrite;
    bool _requestTooLarge;
    bool _requestInvalid;
    bool _cgiPending;
    bool _cgiResponse;
    bool _cgiSucceeded;
    CommonGatewayInterface _cgi;
};
