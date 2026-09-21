#pragma once

#include "enum/HttpStatus.hpp"
#include "http/HttpException.hpp"
#include "http/HttpResponse.hpp"
#include "http/HttpRequest.hpp"
#include "enum/HttpMethod.hpp"
#include "Config.hpp"
#include "CommonGatewayInterface.hpp"
#include "utils.hpp"
#include <fstream>
#include <unistd.h>
#include <sys/stat.h>

class RequestHandler {
  public:
    RequestHandler(HttpRequest req, int socket, const Config& config,
             const std::string& remoteAddress = "0.0.0.0");
    void handleMethod();
    bool prepareCgi(CommonGatewayInterface& cgi);
    HttpResponse handleCgiOutput(const std::string& output);
    ~RequestHandler();
    class Forbidden : public HttpException {
      public:
        Forbidden(int socket): HttpException(HttpStatus::FORBIDDEN, socket) {}
    };
    class NotFound : public HttpException {
      public:
        NotFound(int socket): HttpException(HttpStatus::NOT_FOUND, socket) {}
    };
    class MethodNotAllowed : public HttpException {
      public:
        MethodNotAllowed(int socket): HttpException(HttpStatus::METHOD_NOT_ALLOWED, socket) {}
    };
    class InternalServerError : public HttpException {
      public:
        InternalServerError(int socket): HttpException(HttpStatus::INTERNAL_SERVER_ERROR, socket) {}
    };
    class BadRequest : public HttpException {
      public:
        BadRequest(int socket): HttpException(HttpStatus::BAD_REQUEST, socket) {}
    };
    class NoContent : public HttpException {
      public:
        NoContent(int socket): HttpException(HttpStatus::NO_CONTENT, socket) {}
    };
  private:
    HttpRequest _req;
    int _socket;
    std::string _remoteAddress;
    const Config& _config;

    const std::string& getContentTypeOfPath(std::string path) const;
    std::string resolvePath(const std::string& requestPath) const;
    std::string resolveConfiguredFile() const;
    bool isCgi(const std::string& path) const;
    std::string configuredValue(const std::string& key) const;
    bool isDirectory(std::string path) const;
    HttpResponse handleGet();
    HttpResponse handlePost();
    HttpResponse handlePut();
    HttpResponse handleDelete();
    // Converts raw CGI headers and body into the server-owned HTTP response.
    HttpResponse parseCgiOutput(const std::string& output);
};
