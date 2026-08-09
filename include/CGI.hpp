#pragma once

# include <iostream>
# include <sys/types.h>
# include <sys/wait.h>
# include <unistd.h>

class Cgi {
  public:
    Cgi();
    Cgi(const Cgi& other);
    Cgi& operator=(const Cgi& other);
    ~Cgi();
    void	processInput(std::string input);
    void	createSubprocess(const std::string& filename, const std::string& args);
};


