#include "HttpResponse.hpp"
#include "Logger.hpp"
#include "enum/HttpStatus.hpp"

HttpResponse::HttpResponse(std::string _body, HttpStatus::Code _status, int socket, std::string contentType)
  : _body(_body), _status(_status),  _contentType(contentType), _socket(socket) {
    _response = serialize();
}


std::string HttpResponse::serialize() {
    std::stringstream res;
    res << "HTTP/1.1 " << _status << " " << getSentenceResponseHttpStatus(_status) << "\r\n";
    res << "Content-Length: " << _body.length() << "\r\n";
    res << "Content-Type: " << _contentType << "\r\n";
    res << "\r\n";
    res << _body; // BUG: can't display pictures as strings, terminal fills with nonsense
    return res.str();
}

std::string HttpResponse::getSentenceResponseHttpStatus(HttpStatus::Code status) const {
  switch (status) {
    case HttpStatus::CONTINUE: return "Continue";
    case HttpStatus::SWITCHING_PROTOCOLS: return "Switching Protocols";
    case HttpStatus::OK: return "OK";
    case HttpStatus::CREATED: return "Created";
    case HttpStatus::ACCEPTED: return "Accepted";
    case HttpStatus::NON_AUTHORITATIVE_INFORMATION: return "Non-authoritative Information";
    case HttpStatus::NO_CONTENT: return "No Content";
    case HttpStatus::RESET_CONTENT: return "Reset Content";
    case HttpStatus::PARTIAL_CONTENT: return "Partial Content";
    case HttpStatus::MULTIPLE_CHOICES: return "Multiple Choices";
    case HttpStatus::MOVED_PERMANENTLY: return "Moved Permanently";
    case HttpStatus::FOUND: return "Found";
    case HttpStatus::SEE_OTHER: return "See Other";
    case HttpStatus::NOT_MODIFIED: return "Not Modified";
    case HttpStatus::USE_PROXY: return "Use Proxy";
    case HttpStatus::TEMPORARY_REDIRECT: return "Temporary Redirect";
    case HttpStatus::BAD_REQUEST: return "Bad Request";
    case HttpStatus::UNAUTHORIZED: return "Unauthorized";
    case HttpStatus::PAYMENT_REQUIRED: return "Payment Required";
    case HttpStatus::FORBIDDEN: return "Forbidden";
    case HttpStatus::NOT_FOUND: return "Not Found";
    case HttpStatus::METHOD_NOT_ALLOWED: return "Method Not Allowed";
    case HttpStatus::NOT_ACCEPTABLE: return "Not Acceptable";
    case HttpStatus::PROXY_AUTHENTICATION_REQUIRED: return "Proxy Authentication Required";
    case HttpStatus::REQUEST_TIMEOUT: return "Request Timeout";
    case HttpStatus::CONFLICT: return "Conflict";
    case HttpStatus::GONE: return "Gone";
    case HttpStatus::LENGTH_REQUIRED: return "Length Required";
    case HttpStatus::PRECONDITION_FAILED: return "Precondition Failed";
    case HttpStatus::PAYLOAD_TOO_LARGE: return "Payload Too Large";
    case HttpStatus::URI_TOO_LONG: return "URI Too Long";
    case HttpStatus::UNSUPPORTED_MEDIA_TYPE: return "Unsupported Media Type";
    case HttpStatus::RANGE_NOT_SATISFIABLE: return "Range Not Satisfiable";
    case HttpStatus::EXPECTATION_FAILED: return "Expectation Failed";
    case HttpStatus::UPGRADE_REQUIRED: return "Upgrade Required";
    case HttpStatus::INTERNAL_SERVER_ERROR: return "Internal Server Error";
    case HttpStatus::BAD_GATEWAY: return "Bad Gateway";
    case HttpStatus::SERVICE_UNAVIABLE: return "Service Unavailable";
    case HttpStatus::GATEWAY_TIMEOUT: return "Gateway Timeout";
    case HttpStatus::HTTP_VERSION_NOT_SUPPORTED: return "Http Version Not Supported";
    default: return "Unknown Status";
  }
}

void HttpResponse::sendHttpResponse() {
  Logger::debug("Sending HTTP response as:\n" + _response);
  send(_socket, _response.c_str(), _response.length(), 0);
}

HttpResponse::~HttpResponse() {
}
