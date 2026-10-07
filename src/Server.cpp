#include "Server.hpp"
#include "Logger.hpp"
#include "utils.hpp"
#include <arpa/inet.h>
#include <cstring>
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

Server::Server(const Config& config): _port(config.server().port), _config(config) {
  Logger::info("Server Created");
  std::vector<Config::ListenerConfig> listeners = config.server().listeners;
  if (listeners.empty()) {
    Config::ListenerConfig fallback;
    fallback.host = config.server().host;
    fallback.port = config.server().port;
    listeners.push_back(fallback);
  }

  for (std::size_t i = 0; i < listeners.size(); ++i) {
    sockaddr_in serverAddress;
    std::memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(listeners[i].port);
    serverAddress.sin_addr.s_addr = listeners[i].host.empty() || listeners[i].host == "0.0.0.0"
      ? INADDR_ANY : inet_addr(listeners[i].host.c_str());

    int listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket == -1)
      throw std::runtime_error("socket: " + std::string(strerror(errno)));
    fcntl(listenSocket, F_SETFL, O_NONBLOCK);

    int opt = 1;
    if (setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
      close(listenSocket);
      throw std::runtime_error("setsockopt: " + std::string(strerror(errno)));
    }
    if (bind(listenSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
      close(listenSocket);
      throw std::runtime_error("bind: " + std::string(strerror(errno)));
    }
    if (listen(listenSocket, 5) == -1) {
      close(listenSocket);
      throw std::runtime_error("listen: " + std::string(strerror(errno)));
    }
    _listenSockets.push_back(listenSocket);
    _serverAddresses.push_back(serverAddress);
    Logger::info("Socket Created at " + listeners[i].host + ":" + to_string(listeners[i].port));
  }
}

Server::~Server() {
  for (std::size_t i = 0; i < _listenSockets.size(); ++i)
    close(_listenSockets[i]);
  Logger::info("Server Destroyed");
}

// POLLINT -> read request
// POLLOUT -> write response
void Server::run() {
  signal(SIGPIPE, SIG_IGN);
  signal(SIGINT, handlerSignal);
  std::map<int, Client> clients;
  // Catch Crtl + C signal
  while (eventLoop) {
    std::vector<struct pollfd> pollFds;
    std::vector<int> clientsToRemove;

    for (std::size_t i = 0; i < _listenSockets.size(); ++i) {
      struct pollfd serverFd;
      serverFd.fd = _listenSockets[i];
      serverFd.events = POLLIN;
      pollFds.push_back(serverFd);
    }

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
    int timeout = _config.server().timeout;
    int ret = poll(&pollFds[0], pollFds.size(), timeout);
    if (ret < 0) {
      Logger::error("poll: " + std::string(strerror(errno)));
      if (!eventLoop)
        break;
    }
    if (ret == 0) {
      continue;
    }

    for (std::map<int, Client>::iterator client = clients.begin();
         client != clients.end(); ++client) {
      if (client->second.hasTimedOut()) {
        Logger::warn("Client timed out: " + to_string(client->first));
        client->second.closeConnection();
        clientsToRemove.push_back(client->first);
        continue;
      }
      if (!client->second.isCgiPending())
        continue;
      int inputFd = client->second.getCgi().getInputFd();
      int outputFd = client->second.getCgi().getOutputFd();
      for (size_t i = 0; i < pollFds.size(); ++i) {
        if (pollFds[i].fd == inputFd &&
            (pollFds[i].revents & (POLLOUT | POLLERR | POLLHUP)))
          client->second.getCgi().writeInput();
        if (pollFds[i].fd == outputFd &&
            (pollFds[i].revents & (POLLIN | POLLHUP | POLLERR)))
          client->second.getCgi().readOutput();
      }
      if (client->second.getCgi().isFinished() &&
          client->second.getCgi().getOutputFd() == -1)
        client->second.finishCgi(client->second.getCgi().succeeded());
    }

    for (size_t i = 0; i < pollFds.size(); ++i) {
      bool isListenSocket = false;
      for (std::size_t j = 0; j < _listenSockets.size(); ++j) {
        if (pollFds[i].fd == _listenSockets[j]) {
          isListenSocket = true;
          break;
        }
      }
      if (!isListenSocket && clients.find(pollFds[i].fd) == clients.end())
        continue;
      if (pollFds[i].revents & POLLIN) {
        if (isListenSocket) {
          sockaddr_in remoteAddress;
          socklen_t remoteAddressLength = sizeof(remoteAddress);
          int newClientFd = accept(pollFds[i].fd,
            reinterpret_cast<sockaddr *>(&remoteAddress), &remoteAddressLength);
          fcntl(newClientFd, F_SETFL, O_NONBLOCK);
          if (newClientFd == -1) {
            Logger::error("accept: " + std::string(strerror(errno)));
            continue;
          }
          const unsigned char *addressBytes =
            reinterpret_cast<const unsigned char *>(&remoteAddress.sin_addr.s_addr);
          std::string remoteAddressText = to_string(static_cast<unsigned int>(addressBytes[0])) + "." +
            to_string(static_cast<unsigned int>(addressBytes[1])) + "." +
            to_string(static_cast<unsigned int>(addressBytes[2])) + "." +
            to_string(static_cast<unsigned int>(addressBytes[3]));
          std::size_t maxRequestSize = _config.server().max_body_size;
          if (maxRequestSize <= static_cast<std::size_t>(-1) - 65536)
            maxRequestSize += 65536;
          Client newClient(newClientFd, remoteAddressText, maxRequestSize,
            _config.server().max_body_size, _config.server().timeout);
          clients[newClientFd] = newClient;
        } else {
          int clientSock = pollFds[i].fd;
          int isAlive = clients[clientSock].readRequest();
          if (!isAlive) {
            clients[clientSock].closeConnection();
            clientsToRemove.push_back(clientSock);
          }
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
        RequestHandler handler(req, clientSocket, _config,
          clients[clientSocket].getRemoteAddress());
        bool cgiStarted = false;
        try {
          if (clients[clientSocket].isRequestInvalid())
            throw HttpException(HttpStatus::BAD_REQUEST, clientSocket);
          if (clients[clientSocket].isRequestTooLarge())
            throw HttpException(HttpStatus::PAYLOAD_TOO_LARGE, clientSocket);
          handler.validateRequest();
          if (clients[clientSocket].hasCgiResponse()) {
            if (clients[clientSocket].cgiSucceeded())
              handler.handleCgiOutput(clients[clientSocket].getCgi().getOutput()).sendHttpResponse();
            else {
              Logger::error("CGI execution failed (502 - Bad Gateway)");
              throw HttpException(HttpStatus::BAD_GATEWAY, clientSocket);
            }
          } else if (handler.prepareCgi(clients[clientSocket].getCgi())) {
            clients[clientSocket].startCgi();
            cgiStarted = true;
          } else
            handler.handleMethod();
        } catch (const HttpException& e) {
          Logger::warn("HttpException: " + to_string(e.what()));
          e.SendExceptionResponse(_config.errorPage(e.statusCode(), req.getPath()));
        } catch (const std::exception& e) {
          Logger::error("Exception: " + to_string(e.what()));
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
  }
}
