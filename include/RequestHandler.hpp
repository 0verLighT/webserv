#pragma once

#include "HttpResponse.hpp"
#include "HttpRequest.hpp"
#include "enum/HttpMethod.hpp"

class RequestHandler {
  public:
    RequestHandler(HttpRequest req, int socket);
    void handleMethod();
    ~RequestHandler();
  private:
    HttpRequest _req;
    int _socket;

    const std::string& getContentTypeOfPath(std::string path) const;
    HttpResponse handleGet();
    HttpResponse handlePost();
    HttpResponse handlePut();
    HttpResponse handleDelete();
};
