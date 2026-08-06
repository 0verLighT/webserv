#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "enum/HttpMethod.hpp"
#include "RequestHandler.hpp"
#include "Logger.hpp"
#include "enum/HttpStatus.hpp"
#include "utils.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <map>

RequestHandler::RequestHandler(HttpRequest req, int socket) : _req(req), _socket(socket) {}

void RequestHandler::handleMethod() {
  Logger::debug("Id method : " + to_string(static_cast<int>(this->_req.getMethod())));
  HttpResponse res("", HttpStatus::METHOD_NOT_ALLOWED, this->_socket, "text/plain");
  switch (static_cast<int>(this->_req.getMethod())) {
    case HttpMethod::GET : 
      Logger::debug("GET : " + to_string(this->_req.getMethod()));
      res = handleGet();
      break;
    case HttpMethod::POST: 
      Logger::debug("POST : " + to_string(this->_req.getMethod()));
      res = handlePost();
      break;
    case HttpMethod::PUT:
      Logger::debug("PUT : " + to_string(this->_req.getMethod()));
      res = handlePut();
      break;
    case HttpMethod::DELETE:
      Logger::debug("DELETE : " + to_string(this->_req.getMethod()));
      res = handleDelete();
      break;
      // In this case Thows 405 Method Not Allowed to the Client 
    default:
      Logger::debug("DEFAULT : " + to_string(this->_req.getMethod()));
      break;
  }
  res.sendHttpResponse();
}

HttpResponse RequestHandler::handleGet() {
  Logger::debug("Handling GET " + _req.getPath());
  std::string path = "./html" + _req.getPath();
  if (_req.getPath().find("..") != std::string::npos) {
    return HttpResponse("", HttpStatus::FORBIDDEN, _socket, "text/html");
  }
  // Fallback on 127.0.0.1:8080/ -> 127.0.0.1:8080/index.html
  if (_req.getPath().find(".") == std::string::npos) {
    path += "index.html";
  }
  std::ifstream file(path.c_str());

  if (!file.is_open()) {
    std::string path = "./html/" +  to_string(HttpStatus::NOT_FOUND) + ".html";
    std::ifstream file(path.c_str());
    std::stringstream body;
    body << file.rdbuf();
    return HttpResponse(body.str(), HttpStatus::NOT_FOUND, _socket, "text/html");
    
  }

  std::stringstream body;
  body << file.rdbuf();
  return HttpResponse(body.str(), HttpStatus::OK, _socket, getContentTypeOfPath(path));
}

HttpResponse RequestHandler::handlePost() {
  Logger::debug("Handling POST " + _req.getPath());
  
  return HttpResponse("", HttpStatus::CREATED, _socket, "text/plain");
}

HttpResponse RequestHandler::handlePut() {
  Logger::debug("Handling PUT " + _req.getPath());
  return HttpResponse("Peak", HttpStatus::OK, _socket, "text/plain");
}

HttpResponse RequestHandler::handleDelete() {
  Logger::debug("Handling DELETE " + _req.getPath());
  return HttpResponse("No Content", HttpStatus::NO_CONTENT, _socket, "text/html");
}

static const std::map<std::string, std::string> miniTable() {
  static std::map<std::string, std::string> contentType;

  if (contentType.empty()) {
    contentType[".aac"] = "audio/aac";
    contentType[".abw"] = "application/x-abiword";
    contentType[".apng"] = "image/apng";
    contentType[".arc"] = "application/x-freearc";
    contentType[".avif"] = "image/avif";
    contentType[".avi"] = "video/x-msvideo";
    contentType[".azw"] = "application/vnd.amazon.ebook";
    contentType[".bin"] = "application/octet-stream";
    contentType[".bmp"] = "image/bmp";
    contentType[".bz"] = "application/x-bzip";
    contentType[".bz2"] = "application/x-bzip2";
    contentType[".cda"] = "application/x-cdf";
    contentType[".csh"] = "application/x-csh";
    contentType[".css"] = "text/css";
    contentType[".csv"] = "text/csv";
    contentType[".doc"] = "application/msword";
    contentType[".docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
    contentType[".eot"] = "application/vnd.ms-fontobject";
    contentType[".epub"] = "application/epub+zip";
    contentType[".gz"] = "application/gzip";
    contentType[".gif"] = "image/gif";
    contentType[".htm"] = "text/html";
    contentType[".html"] = "text/html";
    contentType[".ico"] = "image/vnd.microsoft.icon";
    contentType[".ics"] = "text/calendar";
    contentType[".jar"] = "application/java-archive";
    contentType[".jpeg"] = "image/jpeg";
    contentType[".jpg"] = "image/jpeg";
    contentType[".js"] = "text/javascript";
    contentType[".json"] = "application/json";
    contentType[".jsonld"] = "application/ld+json";
    contentType[".md"] = "text/markdown";
    contentType[".mid"] = "audio/midi";
    contentType[".midi"] = "audio/midi";
    contentType[".mjs"] = "text/javascript";
    contentType[".m4a"] = "audio/mp4";
    contentType[".mp3"] = "audio/mpeg";
    contentType[".mp4"] = "video/mp4";
    contentType[".mpeg"] = "video/mpeg";
    contentType[".mpkg"] = "application/vnd.apple.installer+xml";
    contentType[".odp"] = "application/vnd.oasis.opendocument.presentation";
    contentType[".ods"] = "application/vnd.oasis.opendocument.spreadsheet";
    contentType[".odt"] = "application/vnd.oasis.opendocument.text";
    contentType[".oga"] = "audio/ogg";
    contentType[".ogv"] = "video/ogg";
    contentType[".ogx"] = "application/ogg";
    contentType[".opus"] = "audio/ogg";
    contentType[".otf"] = "font/otf";
    contentType[".png"] = "image/png";
    contentType[".pdf"] = "application/pdf";
    contentType[".php"] = "application/x-httpd-php";
    contentType[".ppt"] = "application/vnd.ms-powerpoint";
    contentType[".pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
    contentType[".rar"] = "application/vnd.rar";
    contentType[".rtf"] = "application/rtf";
    contentType[".sh"] = "application/x-sh";
    contentType[".svg"] = "image/svg+xml";
    contentType[".tar"] = "application/x-tar";
    contentType[".tif"] = "image/tiff";
    contentType[".tiff"] = "image/tiff";
    contentType[".ts"] = "video/mp2t";
    contentType[".ttf"] = "font/ttf";
    contentType[".txt"] = "text/plain";
    contentType[".vsd"] = "application/vnd.visio";
    contentType[".wav"] = "audio/wav";
    contentType[".weba"] = "audio/webm";
    contentType[".webm"] = "video/webm";
    contentType[".webmanifest"] = "application/manifest+json";
    contentType[".webp"] = "image/webp";
    contentType[".woff"] = "font/woff";
    contentType[".woff2"] = "font/woff2";
    contentType[".xhtml"] = "application/xhtml+xml";
    contentType[".xls"] = "application/vnd.ms-excel";
    contentType[".xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
    // RFC 7303 recommended "application/xml" instead of "text/xml"
    contentType[".xml"] = "application/xml";
    contentType[".xul"] = "application/vnd.mozilla.xul+xml";
    contentType[".zip"] = "application/zip";
    // 3gp can be "audio/3gpp" if it doesn't contain video"
    contentType[".3gp"] = "video/3gpp";
    // 3g2 can be "audio/3gpp2" if it doesn't contain video"
    contentType[".3g2"] = "video/3gpp2";
    contentType[".7z"] = "application/x-7z-compressed";
  }
  return contentType;
}

const std::string& RequestHandler::getContentTypeOfPath(std::string path) const {
  Logger::debug(path);
  static const std::string defaultType = "application/octet-stream";
  std::string ext = getExtenstionFormPath(path);
  if (ext.empty())
    return defaultType;
  const std::map<std::string, std::string>& contentType = miniTable();

  std::map<std::string, std::string>::const_iterator it = contentType.find(ext);
  if (it != contentType.end())
    return (it->second);
  
  return defaultType;
}




RequestHandler::~RequestHandler() {}