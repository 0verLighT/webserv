#include "Server.hpp"
#include "Logger.hpp"
#include "utils.hpp"
#include <exception>
#include <sys/fcntl.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <unistd.h>

volatile sig_atomic_t eventLoop = 1;

void handlerSignal(int sig) {
  (void)sig;
  eventLoop = 0;
  Logger::warn("Signal " + to_string(sig) + " received");
}

Server::Server(const Config& config): _port(config.get<int>("port")), _config(config) {
  Logger::info("Server Created");
  _serverAddress.sin_family =  AF_INET;
  _serverAddress.sin_port = htons(_port);
  _serverAddress.sin_addr.s_addr = INADDR_ANY;
  _socket = socket(AF_INET, SOCK_STREAM, 0);
  fcntl(_socket, F_SETFL, O_NONBLOCK);
  Logger::info("Socket Created at " + to_string(_port));
  if (_socket == -1) {
    throw std::runtime_error("socket: " + std::string(strerror(errno)));
  };
  int opt = 1;
  if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    throw std::runtime_error("setsockopt: " + std::string(strerror(errno)));
  }
  if (bind(_socket, (struct sockaddr*)&_serverAddress, sizeof(_serverAddress)) == -1) {
    throw std::runtime_error("bind: " + std::string(strerror(errno)));
  };
  if (listen(_socket, 5) == -1) {
    throw std::runtime_error("listen: " + std::string(strerror(errno)));
  };
}

Server::~Server() {
  close(_socket);
  Logger::info("Server Destroyed");
}

// POLLINT -> read request
// POLLOUT -> write response
void Server::run() {
  signal(SIGINT, handlerSignal);
  std::map<int, Client> clients;
  // Catch Crtl + C signal
  while (eventLoop) {
    std::vector<struct pollfd> pollFds;
    std::vector<int> clientsToRemove;

    struct pollfd serverFd;
    serverFd.fd = _socket;
    serverFd.events = POLLIN;
    pollFds.push_back(serverFd);

    for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it) {
      if (it->second.isCgiPending()) {
        struct pollfd cgiClient;
        cgiClient.fd = it->first;
        cgiClient.events = POLLOUT;
        pollFds.push_back(cgiClient);
        if (it->second.getCgi().getInputFd() != -1) {
          struct pollfd cgiInput;
          cgiInput.fd = it->second.getCgi().getInputFd();
          cgiInput.events = POLLOUT;
          pollFds.push_back(cgiInput);
        }
        if (it->second.getCgi().getOutputFd() != -1) {
          struct pollfd cgiOutput;
          cgiOutput.fd = it->second.getCgi().getOutputFd();
          cgiOutput.events = POLLIN;
          pollFds.push_back(cgiOutput);
        }
        continue;
      }
      struct pollfd clientFd;
      clientFd.fd = it->first;
      if (it->second.getReadTowrite()) {
        clientFd.events = POLLOUT;
      } else {
        clientFd.events = POLLIN;
      }
      pollFds.push_back(clientFd);
    }
    int timeout = 5000;
    int ret = poll(&pollFds[0], pollFds.size(), timeout);
    if (ret < 0) {
      Logger::error("poll: " + std::string(strerror(errno)));
      if (!eventLoop)
        break;
    }
    if (ret == 0) {
      Logger::info("poll : timeout");
      continue;
    }

    for (std::map<int, Client>::iterator client = clients.begin();
         client != clients.end(); ++client) {
      if (!client->second.isCgiPending())
        continue;
      int inputFd = client->second.getCgi().getInputFd();
      int outputFd = client->second.getCgi().getOutputFd();
      for (size_t i = 0; i < pollFds.size(); ++i) {
        if (pollFds[i].fd == inputFd && (pollFds[i].revents & POLLOUT))
          client->second.getCgi().writeInput();
        if (pollFds[i].fd == outputFd &&
            (pollFds[i].revents & (POLLIN | POLLHUP | POLLERR)))
          client->second.getCgi().readOutput();
      }
      client->second.getCgi().readOutput();
      if (client->second.getCgi().isFinished() &&
          client->second.getCgi().getOutputFd() == -1)
        client->second.finishCgi();
    }

    for (size_t i = 0; i < pollFds.size(); ++i) {
      if (pollFds[i].fd != _socket && clients.find(pollFds[i].fd) == clients.end())
        continue;
      if (pollFds[i].revents & POLLIN) {
        if (pollFds[i].fd == _socket) {
          int newClientFd = accept(_socket, NULL, NULL);
          fcntl(newClientFd, F_SETFL, O_NONBLOCK);
          if (newClientFd == -1) {
            Logger::error("accept: " + std::string(strerror(errno)));
            continue;
          }
          Client newClient(newClientFd);
          clients[newClientFd] = newClient;
          Logger::info("New client connected: " + to_string(newClientFd));
        } else {
          int clientSock = pollFds[i].fd;
          int isAlive = clients[clientSock].readRequest();
          if (!isAlive) {
            clients[clientSock].closeConnection();
            clientsToRemove.push_back(clientSock);
          }
          Logger::info("Request read from client: " + to_string(clientSock));
        }
      }
      if (pollFds[i].revents & POLLOUT) {
        int clientSocket = pollFds[i].fd;
        if (clients[clientSocket].isCgiPending())
          continue;
        HttpRequest req;

        req.parseRequest(clients[clientSocket].getReqBuffer());
        // Pass the same immutable configuration used to initialize this
        // server, so request handling never reparses the configuration file.
        RequestHandler handler(req, clientSocket, _config);
        bool cgiStarted = false;
        try {
          if (clients[clientSocket].hasCgiResponse()) {
            handler.handleCgiOutput(clients[clientSocket].getCgi().getOutput()).sendHttpResponse();
          } else if (handler.prepareCgi(clients[clientSocket].getCgi())) {
            clients[clientSocket].startCgi();
            cgiStarted = true;
          } else
            handler.handleMethod();
        } catch (const HttpException& e) {
          Logger::error("Httpexecption :" + to_string(e.what()));
          e.SendExecptionResponse();
        } catch (const std::exception& e) {
          Logger::error("Exception :" + to_string(e.what()));
        }
        if (!cgiStarted) {
          clients[clientSocket].closeConnection();
          clientsToRemove.push_back(clientSocket);
        }
      }
    }
    for (std::vector<int>::iterator it = clientsToRemove.begin(); it != clientsToRemove.end(); ++it) {
      clients.erase(*it);
    }
    // Logger::info("Cycle Event Loop");
  }
}
