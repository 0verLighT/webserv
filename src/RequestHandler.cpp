#include "enum/HttpMethod.hpp"
#include "RequestHandler.hpp"
#include "Logger.hpp"
#include "enum/HttpStatus.hpp"


RequestHandler::RequestHandler(HttpRequest req, HttpResponse res) : _req(req), _res(res) {}

void RequestHandler::handleMethod() {
  Logger::debug("Id method : " + Logger::to_string(static_cast<int>(this->_req.getMethod())));
  switch (static_cast<int>(this->_req.getMethod())) {
    case HttpMethod::GET : 
      Logger::debug("GET : " + Logger::to_string(this->_req.getMethod()));
      break;
    case HttpMethod::POST: 
      Logger::debug("POST : " + Logger::to_string(this->_req.getMethod()));
      break;
    case HttpMethod::PUT:
      Logger::debug("PUT : " + Logger::to_string(this->_req.getMethod()));
      break;
    case HttpMethod::DELETE:
      Logger::debug("DELETE : " + Logger::to_string(this->_req.getMethod()));
      break;
    default: Logger::debug(this->_res.getSentenceResponseHttpStatus(HttpStatus::METHOD_NOT_ALLOWED));
  }
}

RequestHandler::~RequestHandler() {}