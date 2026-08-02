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
    void parseRequest(std::string buffer);
    HttpMethod getMethod() const;
    std::string getBody() const;
    std::map<std::string, std::string> getHeaders() const;
    std::string getHeader(std::string key) const;
    std::string getPath() const;
    std::map<std::string, std::string> parseHeaders(std::string req) const;
    HttpMethod parseMethod(std::string req) const;
    std::string parsePath(std::string req) const;
  private:
    std::map<std::string, HttpMethod> _methodMap;
    HttpMethod _method;
    std::string _path;
    std::map<std::string, std::string> _headers;
    std::string _body;
};
