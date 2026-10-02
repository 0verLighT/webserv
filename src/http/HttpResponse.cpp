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
  if (_socket < 0 || _response.empty())
    return;

  ssize_t result = send(_socket, _response.c_str(), _response.length(), MSG_NOSIGNAL);
  if (result == -1) {
    if (errno == EAGAIN || errno == EWOULDBLOCK)
      return;
    Logger::error("Failed to send response");
    return;
  }

  if (static_cast<std::size_t>(result) < _response.length()) {
    Logger::warn("Response write completed partially on a non-blocking socket");
  }
}

HttpResponse::~HttpResponse() {}
