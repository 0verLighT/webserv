#include "HttpResponse.hpp"
#include "Logger.hpp"

HttpResponse::HttpResponse(std::string _body, HttpStatus _status, int socket)
  : _body(_body), _status(_status), _socket(socket) {
    _response = serialize();
}


std::string HttpResponse::serialize() {
    std::stringstream res;
    res << "HTTP/1.1 " << _status << " " << _status << "\r\n";
    res << "Content-Length: " << _body.length() << "\r\n";
    res << "\r\n";
    res << _body;
    return res.str();
}

void HttpResponse::sendHttpResponse() {
  Logger::debug(_response);
  send(_socket, _response.c_str(), _response.length(), 0);
}


HttpResponse::~HttpResponse() {
}
