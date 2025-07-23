#include "../include/PollManager.hpp"
#include <cstdlib>
#include <iostream>
#include <list>
#include <netinet/in.h>
#include <signal.h>
#include <sstream>
#include <stdio.h>
#include <string>
#include <strings.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

PollManager *PollManager::_instance = NULL;

Server &PollManager::_getServerByEventFd(int socket) {
  for (std::list<Server>::iterator server = _serverList.begin();
       server != _serverList.end(); ++server) {
    // Check here if one of the polls is a server
    if (socket == server->getServerSocket()) {
      return *server;
    }
  }
  throw std::runtime_error("Not a server socket");
}

Server &PollManager::_getServerByClientSocket(int socket) {
  for (std::list<Server>::iterator server = _serverList.begin();
       server != _serverList.end(); ++server) {
    if (server->hasClientSocket(socket)) {
      return *server;
    }
  }

  throw std::runtime_error("Not a client socket");
}

int PollManager::_initEpollWithServers(std::list<Server> &serverList) {
  int epollFd = epoll_create1(0);
  struct epoll_event event;
  if (epollFd == -1) {
    std::cerr << "Failed to create epoll" << std::endl;
    return (-1);
  }

  // Start all the server listener sockets and add their fds to the pfds arr
  for (std::list<Server>::iterator server = serverList.begin();
       server != serverList.end(); ++server) {
    event.events = EPOLLIN;
    event.data.fd = server->getServerSocket();
    // Add the server sockets to a list so we can check the fds easily later
    _serverSocketList.insert(server->getServerSocket());
    if (epoll_ctl(epollFd, EPOLL_CTL_ADD, server->getServerSocket(), &event) ==
        -1) {
      return (-1);
    }
  }
  return (epollFd);
}

PollManager::PollManager(std::list<Server> &serverList) {
  _serverList = serverList;
}

PollManager::~PollManager(void) {}

void PollManager::_sigintHandle(int sigint) {
  (void)sigint;
  // std::cout << "CTRL+C detected! Closing all fds!" << std::endl;
  if (_instance != NULL) {
    for (std::list<Server>::iterator server = _instance->_serverList.begin();
         server != _instance->_serverList.end(); ++server) {
      server->closeAllSockets();
    }
  }
  std::exit(0);
}

std::string PollManager::_recvHeader(int clientSocket) {
  char buffer[2];
  std::string header;
  int b_read = 0;
  do {
    b_read = recv(clientSocket, buffer, 1, MSG_DONTWAIT);
    // std::cout << "bread: " << b_read << std::endl;
    buffer[b_read] = '\0';
    header += buffer;
  } while (b_read > 0 && header.find("\r\n\r\n") == std::string::npos);
  return (header);
}

size_t PollManager::_extractSizeFromHeader(std::string header) {

  size_t clPos = header.find("Content-Length:");
  if (clPos != std::string::npos) {
    size_t start = clPos + 15;
    size_t end = header.find("\r\n", start);
    if (end != std::string::npos) {
      std::stringstream ss;
      size_t ret;
      ss << header.substr(start, end - start);
      ss >> ret;
      return ret;
    }
  }
  return 0;
}

void PollManager::_cleanEpollSocket(Server &server, int clientSocket,
                                    int epollFd) {
  struct epoll_event event;

  close(clientSocket);
  server.delClientSocket(clientSocket);
  epoll_ctl(epollFd, EPOLL_CTL_DEL, clientSocket, &event);
}

void PollManager::_iterateEpollEvents(int epollFd, struct epoll_event *events,
                                      int numEvents) {
  char buffer[1024];
  struct epoll_event event;
  struct sockaddr_in clientAddress;

  for (int i = 0; i < numEvents; ++i) {
    int eventFd = events[i].data.fd;
    if (_serverSocketList.count(eventFd)) {
      Server &server = _getServerByEventFd(eventFd);
      socklen_t clientAddressLength = sizeof(clientAddress);
      int clientSocket =
          accept(server.getServerSocket(), (struct sockaddr *)&clientAddress,
                 &clientAddressLength);
      if (clientSocket == -1) {
        std::cerr << "Detected clientSocket error!" << std::endl;
        perror("accept");
        continue;
      }
      event.events = EPOLLIN;
      event.data.fd = clientSocket;
      if (epoll_ctl(epollFd, EPOLL_CTL_ADD, clientSocket, &event) == -1) {
        std::cerr << "Failed to add client socket to epoll instance."
                  << std::endl;
        close(clientSocket);
        continue;
      }
      server.addClientSocket(clientSocket);
    } else {
      Server &server = _getServerByClientSocket(eventFd);
      int clientSocket = eventFd;
      std::string header = _recvHeader(clientSocket);
      if (header == "") {
        _cleanEpollSocket(server, clientSocket, epollFd);
        continue;
      }

      size_t bodySize = _extractSizeFromHeader(header);
      std::string request = header;

      // Si hay body que leer, leerlo completamente
      if (bodySize > 0) {
        size_t totalReceived = 0;

        while (totalReceived < bodySize) {
          size_t remainingBytes = bodySize - totalReceived;
          size_t bytesToRead = (remainingBytes < sizeof(buffer) - 1) ?
                               remainingBytes : sizeof(buffer) - 1;

          // Usar recv sin MSG_DONTWAIT para lectura bloqueante
          int b_read = recv(clientSocket, buffer, bytesToRead, 0);

          if (b_read <= 0) {
            if (b_read == 0) {
              std::cerr << "Client closed connection before sending complete body" << std::endl;
            } else {
              perror("recv error while reading body");
            }
            break;
          }

          request.append(buffer, b_read);
          totalReceived += b_read;
        }

        // Solo procesar si recibimos el body completo
        if (totalReceived < bodySize) {
          std::cerr << "Incomplete body received: " << totalReceived
                    << "/" << bodySize << " bytes" << std::endl;
          _cleanEpollSocket(server, clientSocket, epollFd);
          continue;
        }
      }

      ClientRequest clientRequest = ClientRequest(request, server.getConfig());
      server.sendResponse(clientRequest, clientSocket);
      _cleanEpollSocket(server, clientSocket, epollFd);
    }
  }
}

void PollManager::start(void) {
  // Here i set the priv property of instance to this so  we can call the signal
  // handler
  _instance = this;
  signal(SIGINT, _sigintHandle);
  struct epoll_event events[MAX_EVENTS];
  // Create all the epoll needed stuff.
  int epollFd = _initEpollWithServers(_serverList);
  if (epollFd == -1) {
    std::cerr << "Failed to add server socket to epoll instance." << std::endl;
    std::exit(1);
  }
  // Main loop
  while (true) {
    int numEvents = epoll_wait(epollFd, events, MAX_EVENTS, -1);
    if (numEvents == -1) {
      std::cerr << "Failed to wait for events." << std::endl;
      break;
    } else
      _iterateEpollEvents(epollFd, events, numEvents);
  }
}
