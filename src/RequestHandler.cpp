#include "CommonGatewayInterface.hpp"
#include "http/HttpRequest.hpp"
#include "http/HttpResponse.hpp"
#include "enum/HttpMethod.hpp"
#include "RequestHandler.hpp"
#include "Logger.hpp"
#include "enum/HttpStatus.hpp"
#include "http/httpUtils.hpp"
#include <algorithm>
#include <cctype>
#include <cerrno>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iterator>
#include <limits.h>
#include <unistd.h>
#include <vector>

static const Config::RouteConfig* matchRoute(const Config& config, const std::string& requestPath);
static bool routeAllowsMethod(const Config::RouteConfig& route, HttpMethod::Code method);
static std::string trimCgiHeaderValue(const std::string& value);

RequestHandler::RequestHandler(HttpRequest req, int socket, const Config& config,
                               const std::string& remoteAddress)
  : _req(req), _socket(socket), _remoteAddress(remoteAddress), _config(config) {}

void RequestHandler::validateRequest() const {
  std::size_t maxBodySize = _config.server().max_body_size;
  const Config::RouteConfig* route = matchRoute(_config, _req.getPath());
  if (route != NULL && route->max_body_size < maxBodySize)
    maxBodySize = route->max_body_size;
  if (_req.getBody().size() > maxBodySize)
    throw HttpException(HttpStatus::PAYLOAD_TOO_LARGE, _socket);
}

void RequestHandler::handleMethod() {
  const Config::RouteConfig* route = matchRoute(_config, this->_req.getPath());
  if (route != NULL && !routeAllowsMethod(*route, this->_req.getMethod()))
    throw MethodNotAllowed(_socket);
  if (route != NULL && !route->redirect.empty()) {
    std::vector<std::pair<std::string, std::string> > headers;
    headers.push_back(std::make_pair("Location", route->redirect));
    HttpResponse("", HttpStatus::FOUND, _socket, "text/plain", headers).sendHttpResponse();
    return;
  }

  HttpResponse res("", HttpStatus::METHOD_NOT_ALLOWED, this->_socket, "text/plain");
  switch (static_cast<int>(this->_req.getMethod())) {
    case HttpMethod::GET :
      // Logger::debug("GET : " + to_string(this->_req.getMethod()));
      res = handleGet(this->_config);
      break;
    case HttpMethod::POST:
      // Logger::debug("POST : " + to_string(this->_req.getMethod()));
      res = handlePost();
      break;
    case HttpMethod::DELETE:
      // Logger::debug("DELETE : " + to_string(this->_req.getMethod()));
      res = handleDelete();
      break;
    default:
      throw MethodNotAllowed(_socket);
  }
  res.sendHttpResponse();
}

static const Config::RouteConfig* matchRoute(const Config& config, const std::string& requestPath) {
  return config.routeForPath(requestPath);
}

static bool routeAllowsMethod(const Config::RouteConfig& route, HttpMethod::Code method) {
  if (route.methods.empty())
    return true;

  std::string methodName;
  switch (method) {
    case HttpMethod::GET:
      methodName = "GET";
      break;
    case HttpMethod::POST:
      methodName = "POST";
      break;
    case HttpMethod::DELETE:
      methodName = "DELETE";
      break;
    default:
      return false;
  }

  for (std::vector<std::string>::const_iterator it = route.methods.begin();
       it != route.methods.end(); ++it) {
    if (*it == methodName)
      return true;
  }
  return false;
}

bool RequestHandler::isDirectory(std::string path) const {
  struct stat st;

  if (stat(path.c_str(), &st) != 0) {
    Logger::warn("stat: " + path + ": " + std::string(strerror(errno)));
    return false;
  }
  return S_ISDIR(st.st_mode);
}

// Create an absolute path from input
std::string RequestHandler::resolvePath(const std::string& requestPath) const {
  if (requestPath.empty() || requestPath[0] != '/' ||
      requestPath.find("..") != std::string::npos)
    throw Forbidden(_socket);

  const Config::RouteConfig* route = matchRoute(_config, requestPath);

  if (requestPath == "/" && _config.server().file.empty() == false && (route == NULL || route->default_file.empty()))
    return resolveConfiguredFile();

  char currentDirectory[PATH_MAX];
  if (getcwd(currentDirectory, sizeof(currentDirectory)) == NULL)
    throw InternalServerError(_socket);

  std::string root = _config.server().root.empty() ? "html" : _config.server().root;
  if (route != NULL && !route->root.empty())
    root = route->root;

  if (route != NULL && !route->default_file.empty()) {
    std::string routePath = route->path;
    while (routePath.size() > 1 && routePath[routePath.size() - 1] == '/')
      routePath.erase(routePath.size() - 1);
    if (requestPath == routePath || requestPath == (routePath + "/")) {
      std::string defaultFile = route->default_file;
      if (defaultFile[0] == '/')
        return defaultFile;
      std::string defaultPath = std::string(currentDirectory) + "/" + root;
      if (!defaultPath.empty() && defaultPath[defaultPath.size() - 1] != '/')
        defaultPath += "/";
      defaultPath += defaultFile;
      struct stat st;
      if (stat(defaultPath.c_str(), &st) == 0)
        return defaultPath;
    }
  }

  std::string relativePath = requestPath;
  if (route != NULL && route->path != "/") {
    std::string routePath = route->path;
    while (routePath.size() > 1 && routePath[routePath.size() - 1] == '/')
      routePath.erase(routePath.size() - 1);
    relativePath = requestPath.substr(routePath.size());
    if (relativePath.empty())
      relativePath = "/";
  }

  std::string documentRootPath = std::string(currentDirectory) + "/" + root + relativePath;
  struct stat st;
  if (stat(documentRootPath.c_str(), &st) == 0)
    return documentRootPath;

  std::string filesystemPath = std::string(currentDirectory) + relativePath;
  if (stat(filesystemPath.c_str(), &st) == 0)
    return filesystemPath;
  return documentRootPath;
}

std::string RequestHandler::resolveConfiguredFile() const {
  std::string configuredFile = configuredValue("file");
  if (configuredFile.empty() || configuredFile.find("..") != std::string::npos)
    throw Forbidden(_socket);

  char currentDirectory[PATH_MAX];
  if (getcwd(currentDirectory, sizeof(currentDirectory)) == NULL)
    throw InternalServerError(_socket);

  if (configuredFile[0] != '/')
    return std::string(currentDirectory) + "/" + configuredFile;

  // Accept a URL-style landing path such as /index.html as well as an absolute filesystem path.
  // Prefer the document-root interpretation when that file exists.
  std::string documentRootPath = std::string(currentDirectory) + "/html" + configuredFile;
  struct stat st;
  if (stat(documentRootPath.c_str(), &st) == 0)
    return documentRootPath;
  return configuredFile;
}

// CGI is enabled only for the configured legacy landing script or a matched route extension.
bool RequestHandler::isCgi(const std::string& path) const {
  const Config::RouteConfig* route = matchRoute(_config, _req.getPath());
  if (route == NULL || !route->cgi_enabled || !_config.server().cgi_enabled)
    return false;

  bool isConfiguredFile = !_config.server().file.empty() && path == resolveConfiguredFile();
  bool extensionMatches = false;
  if (!route->cgi_extension.empty() && path.size() >= route->cgi_extension.size()) {
    extensionMatches = path.compare(path.size() - route->cgi_extension.size(),
                                    route->cgi_extension.size(), route->cgi_extension) == 0;
  }
  if (!isConfiguredFile && !extensionMatches)
    return false;

  struct stat st;
  if (stat(path.c_str(), &st) != 0 || !S_ISREG(st.st_mode))
    return false;
  if (isConfiguredFile && !_config.server().executor.empty())
    return true;
  if (route != NULL && (!route->executor.empty() || !_config.server().executor.empty()))
    return true;
  return access(path.c_str(), X_OK) == 0;
}

std::string RequestHandler::configuredValue(const std::string& key) const {
  std::string value = _config.get<std::string>(key);
  if (value.size() >= 2 && value[0] == '"' && value[value.size() - 1] == '"')
    value = value.substr(1, value.size() - 2);
  return value;
}

bool RequestHandler::prepareCgi(CommonGatewayInterface& cgi) {
  const Config::RouteConfig* route = matchRoute(_config, _req.getPath());
  if (route != NULL && !route->redirect.empty())
    return false;
  if (route != NULL && !routeAllowsMethod(*route, _req.getMethod()))
    throw MethodNotAllowed(_socket);
  std::string path = resolvePath(_req.getPath());
  if (!isCgi(path))
    return false;
  std::string executor = route != NULL && !route->executor.empty() ? route->executor : _config.server().executor;
  cgi.processInput(_req, path, _req.getPath(), _config, _remoteAddress, executor);
  return true;
}

HttpResponse RequestHandler::handleCgiOutput(const std::string& output) {
  return parseCgiOutput(output);
}

HttpResponse RequestHandler::handleGet(const Config& config) {
    std::string path = resolvePath(_req.getPath());
    // Existing static-file logic
//   Logger::info(path);
  if (_req.getPath().find("..") != std::string::npos) {
    throw Forbidden(_socket);
  }
  const Config::RouteConfig* route = matchRoute(config, _req.getPath());
  bool autoindex = route != NULL ? route->autoindex : config.server().autoindex;
  if (isDirectory(path)) {
    if (route != NULL && !route->default_file.empty()) {
      std::string defaultPath = route->default_file[0] == '/' ? route->default_file : path + "/" + route->default_file;
      struct stat defaultStat;
      if (stat(defaultPath.c_str(), &defaultStat) == 0 && S_ISREG(defaultStat.st_mode))
        path = defaultPath;
    }
  }
  if (isDirectory(path)) {
    if (autoindex) {
      std::string autoindexPage = generateAutoindexPage(path);
      if (autoindexPage.empty()) {
        throw NotFound(_socket);
      }
      return HttpResponse(autoindexPage, HttpStatus::OK, _socket, "text/html");
    } else {
      throw Forbidden(_socket);
    }
  }
  struct stat st;
  if (stat(path.c_str(), &st) == -1) {
    if (errno == EACCES) {
      // Logger::debug("Forbidden: " + path);
      throw Forbidden(_socket);
    }
    // Logger::debug("File not found: " + path);
    throw NotFound(_socket);
  }

  std::ifstream file(path.c_str());

  if (!file.is_open()) {
    // Logger::debug("File not found: " + path);
    throw Forbidden(_socket);
  }

  std::stringstream body;
  body << file.rdbuf();
  return HttpResponse(body.str(), HttpStatus::OK, _socket, getContentTypeOfPath(path));
}

HttpResponse RequestHandler::handlePost() {
  const Config::RouteConfig* route = matchRoute(_config, _req.getPath());
  if (route == NULL || route->upload_path.empty())
    throw Forbidden(_socket);
  if (_req.getBody().size() > route->max_body_size)
    throw HttpException(HttpStatus::PAYLOAD_TOO_LARGE, _socket);

  std::string filename;
  std::string fileContent = _req.getBody();
  std::string contentType = _req.getHeader("content-type");
  std::string::size_type multipartPos = contentType.find("multipart/form-data");
  if (multipartPos != std::string::npos) {
    std::string::size_type boundaryPos = contentType.find("boundary=");
    if (boundaryPos == std::string::npos)
      throw BadRequest(_socket);
    std::string boundary = contentType.substr(boundaryPos + 9);
    std::string::size_type semicolon = boundary.find(';');
    if (semicolon != std::string::npos)
      boundary.erase(semicolon);
    boundary = trimCgiHeaderValue(boundary);
    if (boundary.size() >= 2 && boundary[0] == '"' && boundary[boundary.size() - 1] == '"')
      boundary = boundary.substr(1, boundary.size() - 2);
    if (boundary.empty())
      throw BadRequest(_socket);

    std::string delimiter = "--" + boundary;
    std::string::size_type partStart = _req.getBody().find(delimiter);
    if (partStart == std::string::npos)
      throw BadRequest(_socket);
    partStart = _req.getBody().find("\r\n", partStart);
    if (partStart == std::string::npos)
      throw BadRequest(_socket);
    ++partStart;
    std::string::size_type headersEnd = _req.getBody().find("\r\n\r\n", partStart);
    if (headersEnd == std::string::npos)
      throw BadRequest(_socket);

    std::string partHeaders = _req.getBody().substr(partStart, headersEnd - partStart);
    std::string loweredHeaders = partHeaders;
    for (std::string::size_type i = 0; i < loweredHeaders.size(); ++i)
      loweredHeaders[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(loweredHeaders[i])));
    std::string::size_type disposition = loweredHeaders.find("content-disposition:");
    std::string::size_type filenamePos = disposition == std::string::npos ? std::string::npos : loweredHeaders.find("filename=", disposition);
    if (filenamePos == std::string::npos)
      throw BadRequest(_socket);
    filenamePos += 9;
    if (filenamePos >= partHeaders.size())
      throw BadRequest(_socket);
    char quote = partHeaders[filenamePos] == '"' ? '"' : '\0';
    if (quote != '\0')
      ++filenamePos;
    std::string::size_type filenameEnd = quote != '\0' ? partHeaders.find(quote, filenamePos) : partHeaders.find(';', filenamePos);
    if (filenameEnd == std::string::npos)
      filenameEnd = partHeaders.size();
    filename = partHeaders.substr(filenamePos, filenameEnd - filenamePos);

    std::string::size_type dataStart = headersEnd + 4;
    std::string::size_type dataEnd = _req.getBody().find("\r\n" + delimiter, dataStart);
    if (dataEnd == std::string::npos)
      throw BadRequest(_socket);
    fileContent = _req.getBody().substr(dataStart, dataEnd - dataStart);
  } else {
    std::string::size_type routePathLength = route->path.size();
    while (routePathLength > 1 && route->path[routePathLength - 1] == '/')
      --routePathLength;
    if (_req.getPath().size() <= routePathLength || _req.getPath()[routePathLength] != '/')
      throw BadRequest(_socket);
    filename = _req.getPath().substr(routePathLength + 1);
  }

  std::string::size_type separator = filename.find_last_of("/\\");
  if (separator != std::string::npos)
    filename = filename.substr(separator + 1);
  if (filename.empty() || filename == "." || filename == ".." ||
      filename.find_first_of("\r\n") != std::string::npos ||
      filename.find('\0') != std::string::npos)
    throw BadRequest(_socket);

  std::string uploadDirectory = route->upload_path;
  if (uploadDirectory.find("..") != std::string::npos)
    throw Forbidden(_socket);
  if (uploadDirectory[0] != '/') {
    char currentDirectory[PATH_MAX];
    if (getcwd(currentDirectory, sizeof(currentDirectory)) == NULL)
      throw InternalServerError(_socket);
    uploadDirectory = std::string(currentDirectory) + "/" + uploadDirectory;
  }
  struct stat directoryStat;
  if (stat(uploadDirectory.c_str(), &directoryStat) != 0 || !S_ISDIR(directoryStat.st_mode))
    throw InternalServerError(_socket);
  if (access(uploadDirectory.c_str(), W_OK) != 0)
    throw Forbidden(_socket);

  std::string destination = uploadDirectory;
  if (destination[destination.size() - 1] != '/')
    destination += "/";
  destination += filename;
  std::ofstream output(destination.c_str(), std::ios::binary | std::ios::out | std::ios::trunc);
  if (!output.is_open())
    throw InternalServerError(_socket);
  output.write(fileContent.data(), static_cast<std::streamsize>(fileContent.size()));
  if (!output.good())
    throw InternalServerError(_socket);
  output.close();
  return HttpResponse("Uploaded " + filename, HttpStatus::CREATED, _socket, "text/plain");
}

HttpResponse RequestHandler::handleDelete() {
  // Logger::debug("Handling DELETE " + _req.getPath());
  std::string path = resolvePath(_req.getPath());

  if (access(path.c_str(), F_OK) == 0) {
    Logger::info("Attempt to delete `" + _req.getPath() + "`");

    if (access(path.c_str(), W_OK) != 0) {
      Logger::warn("Deletion of file `" + _req.getPath() + "` is forbidden");
      throw Forbidden(_socket);
    }

    int status = std::remove(path.c_str());
    if (status != 0)
      throw InternalServerError(_socket);
  }
  else {
    Logger::warn("File `" + _req.getPath() + "` not found");
    throw NotFound(_socket);
  }

  throw NoContent(_socket);
}

static std::string trimCgiHeaderValue(const std::string& value) {
  std::string::size_type first = value.find_first_not_of(" \t\r");
  if (first == std::string::npos)
    return "";
  std::string::size_type last = value.find_last_not_of(" \t\r");
  return value.substr(first, last - first + 1);
}

static std::string lowercaseCgiHeader(const std::string& value) {
  std::string lower(value);
  for (std::string::size_type i = 0; i < lower.size(); ++i)
    lower[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(lower[i])));
  return lower;
}

static bool parseCgiStatus(const std::string& value, HttpStatus::Code& status) {
  if (value.size() < 3 || value[0] < 48 || value[0] > 57 ||
      value[1] < 48 || value[1] > 57 || value[2] < 48 || value[2] > 57 ||
      (value.size() > 3 && value[3] != 32 && value[3] != 9))
    return false;

  int code = (value[0] - 48) * 100 + (value[1] - 48) * 10 + value[2] - 48;
  if (code < 100 || code > 599)
    return false;
  status = static_cast<HttpStatus::Code>(code);
  return true;
}

static bool isServerOwnedCgiHeader(const std::string& name) {
  return name == "content-length" || name == "connection" ||
         name == "keep-alive" || name == "transfer-encoding" ||
         name == "upgrade" || name == "trailer" || name == "te";
}

HttpResponse RequestHandler::parseCgiOutput(const std::string& output) {
  std::string::size_type separator = output.find("\r\n\r\n");
  std::string::size_type separatorSize = 4;

  // Python print() commonly emits LF-only output.
  if (separator == std::string::npos) {
    separator = output.find("\n\n");
    separatorSize = 2;
  }
    if (separator == std::string::npos) {
      // Body-only CGI scripts use the server's default response metadata.
      return HttpResponse(output, HttpStatus::OK, _socket, "text/plain");
    }

  std::string headers = output.substr(0, separator);
  std::string body = output.substr(separator + separatorSize);
  std::string contentType = "text/plain";
  HttpStatus::Code status = HttpStatus::OK;
  bool hasStatus = false;
  bool hasLocation = false;
  std::vector<std::pair<std::string, std::string> > extraHeaders;

  std::string::size_type lineStart = 0;
  while (lineStart < headers.size()) {
    std::string::size_type lineEnd = headers.find("\n", lineStart);
    if (lineEnd == std::string::npos)
      lineEnd = headers.size();
    std::string line = headers.substr(lineStart, lineEnd - lineStart);
    if (!line.empty() && line[line.size() - 1] == 13)
      line.erase(line.size() - 1);

    std::string::size_type colon = line.find(":");
    if (colon == std::string::npos)
      return HttpResponse("Invalid CGI response header", HttpStatus::BAD_GATEWAY, _socket, "text/plain");

    std::string name = trimCgiHeaderValue(line.substr(0, colon));
    std::string value = trimCgiHeaderValue(line.substr(colon + 1));
    if (name.empty() || name.find_first_of(" \t\r\n") != std::string::npos ||
        value.find_first_of("\r\n") != std::string::npos)
      return HttpResponse("Invalid CGI response header", HttpStatus::BAD_GATEWAY, _socket, "text/plain");

    std::string lowerName = lowercaseCgiHeader(name);
    if (lowerName == "content-type")
      contentType = value;
    else if (lowerName == "status") {
      if (!parseCgiStatus(value, status))
        return HttpResponse("Invalid CGI Status header", HttpStatus::BAD_GATEWAY, _socket, "text/plain");
      hasStatus = true;
    } else if (lowerName == "location") {
      extraHeaders.push_back(std::make_pair(name, value));
      hasLocation = true;
    } else if (!isServerOwnedCgiHeader(lowerName)) {
      // Preserve headers such as Set-Cookie while retaining server framing ownership.
      extraHeaders.push_back(std::make_pair(name, value));
    }

    lineStart = lineEnd + 1;
  }

  // CGI redirects without an explicit Status header default to 302 Found.
  if (hasLocation && !hasStatus)
    status = HttpStatus::FOUND;
  return HttpResponse(body, status, _socket, contentType, extraHeaders);
}

static const std::map<std::string, std::string>& miniTable() {
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
  // Logger::debug(path);
  static const std::string defaultType = "application/octet-stream";
  std::string ext = getExtensionFromPath(path);
  if (ext.empty())
    return defaultType;
  const std::map<std::string, std::string>& contentType = miniTable();

  std::map<std::string, std::string>::const_iterator it = contentType.find(ext);
  if (it != contentType.end())
    return it->second;
  return defaultType;
}



RequestHandler::~RequestHandler() {}
