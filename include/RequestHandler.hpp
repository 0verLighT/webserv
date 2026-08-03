#pragma once

#include "HttpResponse.hpp"
#include "HttpRequest.hpp"
#include "enum/HttpMethod.hpp"

class RequestHandler {
  public:
    RequestHandler(HttpRequest req, HttpResponse res);
    void handleMethod();
    ~RequestHandler();
  private:
    HttpRequest _req;
    HttpResponse _res;
};
