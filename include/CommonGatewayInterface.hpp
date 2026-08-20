#pragma once

# include <iostream>
# include <sys/types.h>
# include <sys/wait.h>
# include <unistd.h>

class CommonGatewayInterface {
  public:
    CommonGatewayInterface();
    CommonGatewayInterface(const CommonGatewayInterface& other);
    CommonGatewayInterface& operator=(const CommonGatewayInterface& other);
    ~CommonGatewayInterface();
    void	processInput(std::string input);
    void	createSubprocess(const std::string& filename, const std::string& args);
};
