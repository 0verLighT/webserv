#include <string>

class HttpRequest {
  public:
    HttpRequest();
    ~HttpRequest();
    std::string ReadRequest(int socket);
};
