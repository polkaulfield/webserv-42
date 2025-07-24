#ifndef SERVER_HPP
#define SERVER_HPP
#include "ClientRequest.hpp"
#include "Config.hpp"
#include "Location.hpp"
#include <list>
#include <poll.h>
#include <string>
#include <sys/epoll.h>
#include <unistd.h>
#define MAX_EVENTS 50

class Server {
private:
  epoll_event _events[MAX_EVENTS];
  std::list<int> _clientSocketList;
  int _serverSocket;
  std::string _endpoint;
  std::list<Location> _locationList;
  bool _isFileUpload;
  Config &_config;

  static void _sigintHandle(int signum);
  int _createServerSocket(int port);
  bool _checkLocation(const ClientRequest &clientRequest);
  std::string _generateCookieId();

public:
  Server();
  Server(Config &config);
  Server(const Server &src);
  ~Server();
  const Server &operator=(const Server &server);

  void printConfig(void);
  int start(void);
  void sendResponse(ClientRequest &clientRequest, int clientSocket);
  void addClientSocket(int clientSocket);
  void delClientSocket(int clientSocket);
  bool hasClientSocket(int clientSocket);
  bool isUploading() const;
  void closeAllSockets(void);
  int getServerSocket();
  const Config &getConfig(void) const;
};
#endif
