#include "http/HttpResponse.hpp"
#include "Logger.hpp"
#include "enum/HttpStatus.hpp"

HttpResponse::HttpResponse(std::string _body, HttpStatus::Code _status, int socket, std::string contentType)
  : _body(_body), _status(_status), _contentType(contentType), _extraHeaders(), _socket(socket) {
    _response = serialize();
}

HttpResponse::HttpResponse(std::string _body, HttpStatus::Code _status, int socket, std::string contentType,
                           const std::vector<std::pair<std::string, std::string> >& extraHeaders)
  : _body(_body), _status(_status), _contentType(contentType),
    _extraHeaders(extraHeaders), _socket(socket) {
    _response = serialize();
}


std::string HttpResponse::serialize() {
    std::stringstream res;
    res << "HTTP/1.1 " << _status << " " << getSentenceResponseHttpStatus(_status) << "\r\n";
    res << "Content-Length: " << _body.length() << "\r\n";
    res << "Content-Type: " << _contentType << "\r\n";
    for (std::vector<std::pair<std::string, std::string> >::const_iterator it =
           _extraHeaders.begin(); it != _extraHeaders.end(); ++it)
      res << it->first << ": " << it->second << "\r\n";
    res << "\r\n";
    res << _body;
    return res.str();
}

void HttpResponse::sendHttpResponse() {
  // Logger::debug(_response);
  size_t sent = 0;
  while (sent < _response.length()) {
    ssize_t result = send(_socket, _response.c_str() + sent, _response.length() - sent, 0);
    if (result == -1) {
      Logger::error("Failed to send response");
      return;
    }
    sent += result;
  }
}

HttpResponse::~HttpResponse() {}
