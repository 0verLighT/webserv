#include "CommonGatewayInterface.hpp"

CommonGatewayInterface::CommonGatewayInterface() {}

CommonGatewayInterface::CommonGatewayInterface(const CommonGatewayInterface& other) {
  *this = other;
}

CommonGatewayInterface& CommonGatewayInterface::operator=(const CommonGatewayInterface& other) {
  (void)other;
  return *this;
}

CommonGatewayInterface::~CommonGatewayInterface() {}

void CommonGatewayInterface::processInput(std::string input) {
  (void)input;
  // end with calling processInput()
}

void CommonGatewayInterface::createSubprocess(const std::string& filepath, const std::string& input) {
  // checks should be done upstream
  pid_t pid;
  int pipefd[2];

  if (pipe(pipefd) == -1)
    throw std::runtime_error("Creating pipe failed.");

  pid = fork();
  if (pid == -1) {
    close(pipefd[0]);
    close(pipefd[1]);
    throw std::runtime_error("Forking process failed.");
  }

  if (pid == 0) {
    // Child process
    close(pipefd[1]);
    if (dup2(pipefd[0], STDIN_FILENO) == -1) {
      close(pipefd[0]);
      throw std::runtime_error("Redirecting stdin failed.");
    }
    close(pipefd[0]);

    char *args[3];
    args[0] = const_cast<char *>(filepath.c_str());
    args[1] = input.empty() ? NULL : const_cast<char *>(input.c_str());
    args[2] = NULL;

    execve(args[0], args, NULL); // envp useless?
    // still in child process, execve failed
    // TODO: properly handle error
    throw std::runtime_error("Execution of CommonGatewayInterface script failed.");
  } else {
    // Parent process
    close(pipefd[0]);
    if (!input.empty()) {
      if (write(pipefd[1], input.c_str(), input.size()) == -1) {
        close(pipefd[1]);
        throw std::runtime_error("Writing to pipe failed.");
      }
    }
    close(pipefd[1]);

    int status;
    if (waitpid(pid, &status, 0) == -1)
      throw std::runtime_error("Waiting for CommonGatewayInterface process failed.");
    if (WIFEXITED(status))
      std::cout << "CommonGatewayInterface script exited with status: " << WEXITSTATUS(status) << std::endl;
    else
      std::cout << "CommonGatewayInterface script did not exit normally." << std::endl;
  }
}
