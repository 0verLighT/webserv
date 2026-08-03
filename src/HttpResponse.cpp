#include "HttpResponse.hpp"
#include "Logger.hpp"
#include "enum/HttpStatus.hpp"

HttpResponse::HttpResponse(std::string _body, HttpStatus::Code _status, int socket)
  : _body(_body), _status(_status), _socket(socket)  {
    _response = serialize();
}


std::string HttpResponse::serialize() {
    std::stringstream res;
    res << "HTTP/1.1 " << _status << " " << _status << "\r\n";
    res << "Content-Length: " << _body.length() << "\r\n";
    res << "Content-Type: " << ContentType::TEXT_PLAIN << "\r\n";
    res << "\r\n";
    res << _body;
    return res.str();
}

std::string HttpResponse::getSentenceResponseHttpStatus(HttpStatus::Code status) const {
  switch (status) {
    case HttpStatus::CONTINUE: return "Continue";
    case HttpStatus::SWITCHING_PROTOCOLS: return "Switching Protocols";
    case HttpStatus::OK: return "OK";
    case HttpStatus::CREATED: return "Created";
    case HttpStatus::ACCEPTED: return "Accepted";
    case HttpStatus::NO_CONTENT: return "No Content";
    case HttpStatus::REDIRECT: return "Redirect";
    case HttpStatus::NOT_FOUND: return "Not Found";
    case HttpStatus::INTERNAL_SERVER_ERROR: return "Internal Server Error";
    case HttpStatus::BAD_GATEWAY: return "Bad Gateway";
    default: return "Unknown Status";
  }
}

void HttpResponse::sendHttpResponse() {
  Logger::debug(_response);
  send(_socket, _response.c_str(), _response.length(), 0);
}

HttpResponse::~HttpResponse() {
}
