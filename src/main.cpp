#include "Server.hpp"
#include <exception>
#include <iostream>
#include "CommonGatewayInterface.hpp"
#include "TomlParser.hpp"

int main(int argc, char **argv) {
  // until Configuration File aren't aviable
  if (argc != 2) {
    std::cerr << "Usage: ./webserv <path/to/config>" << std::endl;
    return 1;
  }
  try {
    TomlParser p;
    p.processInputFile(argv[1]);
    Server Server(p.getValueFromKey<int>("port"));
    Server.run();
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
  }
  return 0;
}
