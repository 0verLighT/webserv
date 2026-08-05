#pragma once

#include "HttpResponse.hpp"
#include "HttpRequest.hpp"
#include "enum/HttpMethod.hpp"

class RequestHandler {
  public:
    RequestHandler(HttpRequest req, int socket);
    void handleMethod();
    std::string getContentTypeOfPath(std::string path) const;
    ~RequestHandler();
  private:
    HttpRequest _req;
    int _socket;

    HttpResponse handleGet();
    HttpResponse handlePost();
    HttpResponse handlePut();
    HttpResponse handleDelete();
};
