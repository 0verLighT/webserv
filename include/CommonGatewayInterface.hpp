#pragma once

# include <iostream>
# include <sys/types.h>
# include <sys/wait.h>
# include <unistd.h>

class CommonGatewayInterface {
  public:
    CommonGatewayInterface();
    ~CommonGatewayInterface();
    void	processInput(std::string input);
    std::string	createSubprocess(const std::string& filename, const std::string& args);
};
