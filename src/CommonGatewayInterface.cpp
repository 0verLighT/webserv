#include "CommonGatewayInterface.hpp"
#include "http/HttpResponse.hpp"
#include "utils.hpp"

CommonGatewayInterface::CommonGatewayInterface() {}

CommonGatewayInterface::~CommonGatewayInterface() {}

void CommonGatewayInterface::processInput(std::string input) {
  (void)input;
}

std::string CommonGatewayInterface::createSubprocess(const std::string& filepath) {
  // checks should be done upstream
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

    char *args[2];
    args[0] = const_cast<char *>(filepath.c_str());
    args[1] = NULL;

    execve(args[0], args, NULL); // envp useless? -> use CGI envp provided by header
    throw std::runtime_error("Execution of CommonGatewayInterface script failed.");
  } else {
    // Parent process
    close(stdinPipe[0]);
    close(stdoutPipe[1]);
    close(stdinPipe[1]);

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
