#include <string>
#include <map>
#include <cstdlib>

enum HttpMethod {
  GET,
  POST,
  PUT,
  DELETE,
  UNKNOWN,
};

class HttpRequest {
  public:
    HttpRequest();
    ~HttpRequest();
    void ReadRequest(int socket);
    HttpMethod GetMethod() const;
    std::string GetBody() const;
    std::map<std::string, std::string> GetHeaders() const;
    std::string GetHeader(std::string key) const;
    std::string GetPath() const;
    std::map<std::string, std::string> ParseHeaders(std::string req) const;
    HttpMethod ParseMethod(std::string req) const;
    std::string ParsePath(std::string req) const;
  private:
    std::map<std::string, HttpMethod> _methodMap;
    HttpMethod _method;
    std::string _path;
    std::map<std::string, std::string> _headers;
    std::string _body;
};
