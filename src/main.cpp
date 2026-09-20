#include "Server.hpp"
#include <exception>
#include <iostream>
#include "CommonGatewayInterface.hpp"
#include "Config.hpp"
#include "TomlParser.hpp"

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Usage: ./webserv <path/to/config>" << std::endl;
    return 1;
  }
  try {
    TomlParser p;
    p.processInputFile(argv[1]);
    Config config(p);
    Server Server(config);
    Server.run();
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
  }
  return 0;
}
