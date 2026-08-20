#include "http/httpUtils.hpp"

std::string getSentenceResponseHttpStatus(HttpStatus::Code status) {
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


void replaceAll(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
}