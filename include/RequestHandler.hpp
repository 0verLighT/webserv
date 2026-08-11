#pragma once

#include "http/HttpException.hpp"
#include "http/HttpResponse.hpp"
#include "http/HttpRequest.hpp"
#include "enum/HttpMethod.hpp"

class RequestHandler {
  public:
    RequestHandler(HttpRequest req, int socket);
    void handleMethod();
    ~RequestHandler();
    class Forbidden : HttpException {
      public:
        Forbidden(): {}
    };
    class NotFound : HttpException {
      public:
        NotFound();
    };
  private:
    HttpRequest _req;
    int _socket;

    const std::string& getContentTypeOfPath(std::string path) const;
    HttpResponse handleGet();
    HttpResponse handlePost();
    HttpResponse handlePut();
    HttpResponse handleDelete();
};
