#include "CommonGatewayInterface.hpp"
#include "http/HttpResponse.hpp"
#include "utils.hpp"
#include <cerrno>
#include "Logger.hpp"

CommonGatewayInterface::CommonGatewayInterface() {}

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

// Retains the original standalone execution path without request-specific CGI data.
std::string CommonGatewayInterface::createSubprocess(const std::string& filepath) {
  _scriptPath = filepath;
  _body.clear();
  _environment.clear();
  _workingDirectory = ".";
  return createSubprocess();
}

// Forks the prepared CGI process and collects its raw stdout for response parsing.
std::string CommonGatewayInterface::createSubprocess() {
  if (_scriptPath.empty())
    throw std::runtime_error("CGI script path is empty.");
  pid_t pid;
  int stdinPipe[2];
  int stdoutPipe[2];

  if (pipe(stdinPipe) == -1 || pipe(stdoutPipe) == -1)
    throw std::runtime_error("CGI pipe creation failed.");

  pid = fork();
  if (pid == -1) {
    close(stdinPipe[0]);
    close(stdinPipe[1]);
    close(stdoutPipe[0]);
    close(stdoutPipe[1]);
    throw std::runtime_error("CGI forking process failed.");
  }

  if (pid == 0) {
    // Child process
    close(stdinPipe[1]);
    if (dup2(stdinPipe[0], STDIN_FILENO) == -1) {
      close(stdinPipe[0]);
      throw std::runtime_error("CGI stdin redirection failed.");
    }
    close(stdinPipe[0]);

    // redirect child's stdout into the pipe so parent can read it
    close(stdoutPipe[0]);
    if (dup2(stdoutPipe[1], STDOUT_FILENO) == -1) {
      close(stdoutPipe[1]);
      throw std::runtime_error("CGI stdout redirection failed.");
    }
    close(stdoutPipe[1]);

    // CGI scripts commonly rely on relative paths.
	// Logger::debug("_workingDirectory = " + _workingDirectory);
    if (chdir(_workingDirectory.c_str()) == -1)
      throw std::runtime_error("CGI working directory change failed.");

    // The script path is argv[0]; interpreter-specific arguments are added later.
    char *args[2];
    args[0] = const_cast<char *>(_scriptPath.c_str());
    args[1] = NULL;

    // execve needs mutable pointers, while the owning strings remain valid until execve.
    std::vector<char *> environment;
    for (std::vector<std::string>::iterator it = _environment.begin();
         it != _environment.end(); ++it)
      environment.push_back(const_cast<char *>(it->c_str()));
    environment.push_back(NULL);

    execve(args[0], args, &environment[0]);
	std::string path = args[0];
	// Logger::debug("filepath = " + path);
    throw std::runtime_error("Execution of CommonGatewayInterface script failed.");
  } else {
    // Parent process
    close(stdinPipe[0]);
    close(stdoutPipe[1]);
    // Feed the decoded HTTP body to CGI stdin, retrying interrupted writes.
    std::string::size_type written = 0;
    while (written < _body.size()) {
      ssize_t bytesWritten = write(stdinPipe[1], _body.c_str() + written, _body.size() - written);
      if (bytesWritten == -1) {
        if (errno == EINTR)
          continue;
        close(stdinPipe[1]);
        close(stdoutPipe[0]);
        waitpid(pid, NULL, 0);
        throw std::runtime_error("Writing CGI stdin failed: " + std::string(strerror(errno)));
      }
      written += static_cast<std::string::size_type>(bytesWritten);
    }
    // CGI expects EOF after the decoded request body.
    close(stdinPipe[1]);

    // Preserve CGI stdout unchanged; the request handler will parse its headers and body.
    std::string output;
    char buffer[4096];
    ssize_t bytesRead;
    while ((bytesRead = read(stdoutPipe[0], buffer, sizeof(buffer))) != 0) {
      if (bytesRead == -1) {
        if (errno == EINTR)
          continue;
        close(stdoutPipe[0]);
        waitpid(pid, NULL, 0);
        throw std::runtime_error(
          "Reading CGI stdout failed: " + std::string(strerror(errno))
        );
      }
      output.append(buffer, bytesRead);
    }

    close(stdoutPipe[0]);

    int status;
    if (waitpid(pid, &status, 0) == -1)
      throw std::runtime_error("Waiting for CommonGatewayInterface process failed.");

    if (WIFEXITED(status))
      Logger::info("CommonGatewayInterface script exited with status: " + to_string(WEXITSTATUS(status)));
    else
      Logger::warn("CommonGatewayInterface script did not exit normally.");

    return (output);
  }
}
