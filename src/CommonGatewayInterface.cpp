#include "CommonGatewayInterface.hpp"
#include "http/HttpResponse.hpp"
#include "utils.hpp"
#include <cerrno>
#include <fcntl.h>
#include "Logger.hpp"

CommonGatewayInterface::CommonGatewayInterface()
  : _bodyOffset(0), _pid(-1), _stdinFd(-1), _stdoutFd(-1) {}

CommonGatewayInterface::~CommonGatewayInterface() {}

// Stores entries in execve-compatible NAME=value form.
void CommonGatewayInterface::addEnvironment(const std::string& name,
                                            const std::string& value) {
  _environment.push_back(name + "=" + value);
}

// CGI requires a textual method even though HttpRequest stores an enum.
std::string CommonGatewayInterface::methodToString(HttpMethod::Code method) {
  switch (method) {
    case HttpMethod::GET: return "GET";
    case HttpMethod::POST: return "POST";
    case HttpMethod::PUT: return "PUT";
    case HttpMethod::DELETE: return "DELETE";
    default: return "";
  }
}

// CGI forwards ordinary request headers using HTTP_ uppercase names.
std::string CommonGatewayInterface::headerToEnvironmentName(const std::string& header) {
  std::string name("HTTP_");
  for (std::string::size_type i = 0; i < header.size(); ++i) {
    if (header[i] == '-')
      name += '_';
    else if (header[i] >= 'a' && header[i] <= 'z')
      name += static_cast<char>(header[i] - 'a' + 'A');
    else
      name += header[i];
  }
  return name;
}

// Converts the server request context into data consumed by the CGI child.
void CommonGatewayInterface::processInput(const HttpRequest& request,
                                          const std::string& scriptPath,
                                          const std::string& scriptName,
                                          const std::string& serverName,
                                          const std::string& serverPort) {
  _scriptPath = scriptPath;
  _body = request.getBody();
  _bodyOffset = 0;
  _output.clear();
  // Do not retain state from a previous request.
  _environment.clear();

  // Relative paths in the script are resolved from its own directory.
  std::string::size_type slash = scriptPath.find_last_of('/');
  _workingDirectory = slash == std::string::npos ? "." : scriptPath.substr(0, slash);
  if (_workingDirectory.empty())
    _workingDirectory = "/";

  // Required CGI metadata plus the request and matched-server context.
  addEnvironment("GATEWAY_INTERFACE", "CGI/1.1");
  addEnvironment("REQUEST_METHOD", methodToString(request.getMethod()));
  addEnvironment("QUERY_STRING", request.getQueryString());
  addEnvironment("SCRIPT_NAME", scriptName);
  addEnvironment("SCRIPT_FILENAME", scriptPath);
  addEnvironment("SERVER_PROTOCOL", "HTTP/" + request.getHttpVersion());
  addEnvironment("SERVER_NAME", serverName);
  addEnvironment("SERVER_PORT", serverPort);

  // CONTENT_* variables are special; all other request headers become HTTP_*.
  std::map<std::string, std::string> headers = request.getHeaders();
  for (std::map<std::string, std::string>::const_iterator it = headers.begin();
       it != headers.end(); ++it) {
    if (it->first == "content-length")
      addEnvironment("CONTENT_LENGTH", it->second);
    else if (it->first == "content-type")
      addEnvironment("CONTENT_TYPE", it->second);
    else
      addEnvironment(headerToEnvironmentName(it->first), it->second);
  }

  // A decoded chunked body has no original Content-Length, so provide its actual size.
  if (headers.find("content-length") == headers.end())
    addEnvironment("CONTENT_LENGTH", to_string(_body.size()));
}

void CommonGatewayInterface::startSubprocess() {
  if (_scriptPath.empty())
    throw std::runtime_error("CGI script path is empty.");
  int stdinPipe[2];
  int stdoutPipe[2];
  if (pipe(stdinPipe) == -1 || pipe(stdoutPipe) == -1)
    throw std::runtime_error("CGI pipe creation failed.");

  _pid = fork();
  if (_pid == -1) {
    close(stdinPipe[0]); close(stdinPipe[1]);
    close(stdoutPipe[0]); close(stdoutPipe[1]);
    throw std::runtime_error("CGI forking process failed.");
  }
  if (_pid == 0) {
    close(stdinPipe[1]);
    close(stdoutPipe[0]);
    if (dup2(stdinPipe[0], STDIN_FILENO) == -1 ||
        dup2(stdoutPipe[1], STDOUT_FILENO) == -1 ||
        chdir(_workingDirectory.c_str()) == -1) {
      char *failureArgs[2];
      failureArgs[0] = const_cast<char *>("/bin/false");
      failureArgs[1] = NULL;
	  // Fails on purpose to replace exit()
      execve(failureArgs[0], failureArgs, NULL);
    }
    close(stdinPipe[0]);
    close(stdoutPipe[1]);

    char *args[2];
    args[0] = const_cast<char *>(_scriptPath.c_str());
    args[1] = NULL;
    std::vector<char *> environment;
    for (std::vector<std::string>::iterator it = _environment.begin();
         it != _environment.end(); ++it)
      environment.push_back(const_cast<char *>(it->c_str()));
    environment.push_back(NULL);
    execve(args[0], args, &environment[0]);
    char *failureArgs[2];
    failureArgs[0] = const_cast<char *>("/bin/false");
    failureArgs[1] = NULL;
	// Fails on purpose to replace exit()
    execve(failureArgs[0], failureArgs, NULL);
  }

  close(stdinPipe[0]);
  close(stdoutPipe[1]);
  _stdinFd = stdinPipe[1];
  _stdoutFd = stdoutPipe[0];
  fcntl(_stdinFd, F_SETFL, O_NONBLOCK);
  fcntl(_stdoutFd, F_SETFL, O_NONBLOCK);
}

bool CommonGatewayInterface::writeInput() {
  if (_stdinFd == -1)
    return true;
  while (_bodyOffset < _body.size()) {
    ssize_t written = write(_stdinFd, _body.c_str() + _bodyOffset,
                            _body.size() - _bodyOffset);
    if (written > 0) {
      _bodyOffset += static_cast<std::string::size_type>(written);
      continue;
    }
    if (written == -1 && errno == EINTR)
      continue;
    if (written == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
      return false;
    close(_stdinFd);
    _stdinFd = -1;
    return true;
  }
  close(_stdinFd);
  _stdinFd = -1;
  return true;
}

bool CommonGatewayInterface::readOutput() {
  char buffer[4096];
  bool closed = false;
  while (_stdoutFd != -1) {
    ssize_t bytesRead = read(_stdoutFd, buffer, sizeof(buffer));
    if (bytesRead > 0) {
      _output.append(buffer, static_cast<std::string::size_type>(bytesRead));
      continue;
    }
    if (bytesRead == -1 && errno == EINTR)
      continue;
    if (bytesRead == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
      break;
    close(_stdoutFd);
    _stdoutFd = -1;
    closed = true;
  }
  return closed;
}

bool CommonGatewayInterface::isFinished() {
  int status;
  if (_pid == -1)
    return true;
  pid_t result = waitpid(_pid, &status, WNOHANG);
  if (result == 0)
    return false;
  _pid = -1;
  if (_stdinFd != -1) { close(_stdinFd); _stdinFd = -1; }
  return true;
}

int CommonGatewayInterface::getInputFd() const { return _stdinFd; }
int CommonGatewayInterface::getOutputFd() const { return _stdoutFd; }
std::string CommonGatewayInterface::getOutput() const { return _output; }
