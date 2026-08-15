#include "Server.hpp"
#include <exception>
#include <iostream>

int main(int argc, char **argv) {
  // until Configuration File aren't aviable
  if (argc != 1) {
    std::cerr << "Usage" << std::endl;
    return 1;
  }
  (void)argv;
  try {
    Server Server(8080);
    Server.run();
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
  }
  return 0;
}
