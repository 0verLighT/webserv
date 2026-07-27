#include "Server.hpp"
#include <exception>
#include <iostream>

int main(int argc, char **argv) {
  (void)argc;
  (void)argv;
  try {
    Server Server(8080);
    Server.run();
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
  }
}