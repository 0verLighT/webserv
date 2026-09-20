#pragma once

# include <iostream>
# include <sys/types.h>
# include <sys/wait.h>
# include <unistd.h>
# include <fcntl.h>
# include <map>
# include <string>
# include <vector>
# include "http/HttpRequest.hpp"
# include "Config.hpp"

class CommonGatewayInterface {
  private:
    // Absolute path to the resolved CGI script passed to execve().
    std::string _scriptPath;
    std::string _executor;
    // Directory made current in the child so the script can use relative paths.
    std::string _workingDirectory;
    // Decoded HTTP request body written to the CGI process standard input.
    std::string _body;
    std::string::size_type _bodyOffset;
    pid_t _pid;
    int _exitStatus;
    bool _finished;
    bool _succeeded;
    int _stdinFd;
    int _stdoutFd;
    std::string _output;
    // CGI NAME=value entries; converted to the envp array required by execve().
    std::vector<std::string> _environment;

    // Adds one NAME=value CGI environment entry to _environment.
    void addEnvironment(const std::string& name, const std::string& value);
    // Converts the project HTTP method enum into its CGI request-method string.
    static std::string methodToString(HttpMethod::Code method);
    // Converts an HTTP header name into its CGI HTTP_HEADER_NAME counterpart.
    static std::string headerToEnvironmentName(const std::string& header);

  public:
    // Creates an empty CGI execution context.
    CommonGatewayInterface();
    // Releases the CGI execution context.
    ~CommonGatewayInterface();
    // Prepares CGI state from a parsed request and the matched script/server context.
    void processInput(const HttpRequest& request, const std::string& scriptPath,
                      const std::string& scriptName,
                      const Config& config,
                      const std::string& remoteAddress);
    // Runs the prepared script and returns its unparsed standard output.
    // Starts CGI without waiting; the server drives these operations from poll().
    void startSubprocess();
    bool writeInput();
    bool readOutput();
    bool isFinished();
    bool succeeded() const;
    int getInputFd() const;
    int getOutputFd() const;
    std::string getOutput() const;
    // Runs an executable without HTTP context; retained for the current CLI test path.
};
