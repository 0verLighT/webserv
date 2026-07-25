#include <cstdio>
#include <iostream>
#include <ostream>
#include <string>
#include <strings.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <fstream>
#include <sstream>
#include <fcntl.h>
#include <unistd.h>

template<typename T>
std::string to_string(const T& value) {
  std::ostringstream oss;
  oss << value;
  return oss.str();
}

std::string readFile(const std::string& filename) {
  std::ifstream file(filename.c_str());
  std::stringstream content;
  content << file.rdbuf();
  return content.str();
}

int main() {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
      perror("sokect");
      return -1;
    }
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
    listen(serverSocket, 5);

    while (1) {
      int clientSocket = accept(serverSocket, NULL, NULL);
      if (clientSocket == -1) {
        continue;
      }
      char buffer[1024] = {0};
      recv(clientSocket, buffer, sizeof(buffer), 0);
      std::cout << "Message from client: " << buffer << std::endl;
      std::string content = readFile("html/index.html");
      std::cout << content << std::endl;
      std::string s = "HTTP/1.1 200 OK\r\nHost: 127.0.0.1:8080\r\nContent-Type: text/html\r\nContent-Length: " + to_string(content.length()) + "\r\n\r\n" + content + "\r\n";
      std::cout << s << std::endl;
      send(clientSocket, s.c_str(), s.size(), 0);
      close(clientSocket);
    }
    close(serverSocket);
    return 0;
}