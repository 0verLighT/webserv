#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "enum/HttpMethod.hpp"
#include "RequestHandler.hpp"
#include "Logger.hpp"
#include "enum/HttpStatus.hpp"
#include <fstream>
#include <sstream>


RequestHandler::RequestHandler(HttpRequest req, int socket) : _req(req), _socket(socket) {}

void RequestHandler::handleMethod() {
  Logger::debug("Id method : " + Logger::to_string(static_cast<int>(this->_req.getMethod())));
  HttpResponse res("", HttpStatus::METHOD_NOT_ALLOWED, this->_socket);
  switch (static_cast<int>(this->_req.getMethod())) {
    case HttpMethod::GET : 
      Logger::debug("GET : " + Logger::to_string(this->_req.getMethod()));
      res = handleGet();
      break;
    case HttpMethod::POST: 
      Logger::debug("POST : " + Logger::to_string(this->_req.getMethod()));
      res = handlePost();
      break;
    case HttpMethod::PUT:
      Logger::debug("PUT : " + Logger::to_string(this->_req.getMethod()));
      res = handlePut();
      break;
    case HttpMethod::DELETE:
      Logger::debug("DELETE : " + Logger::to_string(this->_req.getMethod()));
      res = handleDelete();
      break;
      // In this case Thows 405 Method Not Allowed to the Client 
    default:
      Logger::debug("DEFAULT : " + Logger::to_string(this->_req.getMethod()));
      break;
  }
  res.sendHttpResponse();
}

HttpResponse RequestHandler::handleGet() {
  Logger::debug("Handling GET " + _req.getPath());
  std::string path = "./html" + _req.getPath();
  std::ifstream file(path.c_str());

  if (!file.is_open())
      return HttpResponse("Not Found", HttpStatus::NOT_FOUND, _socket);

  std::stringstream body;
  body << file.rdbuf();
  return HttpResponse(body.str(), HttpStatus::OK, _socket);
}

HttpResponse RequestHandler::handlePost() {
  Logger::debug("Handling POST " + _req.getPath());
  
  return HttpResponse("", HttpStatus::CREATED, _socket);
}

HttpResponse RequestHandler::handlePut() {
  Logger::debug("Handling PUT " + _req.getPath());
  return HttpResponse("Peak", HttpStatus::OK, _socket);
}

HttpResponse RequestHandler::handleDelete() {
  Logger::debug("Handling DELETE " + _req.getPath());
  return HttpResponse("No Content", HttpStatus::NO_CONTENT, _socket);
}


RequestHandler::~RequestHandler() {}