#ifndef SERVER_HPP
# define SERVER_HPP
#include <string>
#include <sys/epoll.h>
#include <unistd.h>
#include "clientRequest.hpp"
#include "location.hpp"
#include "config.hpp"
#include <list>
#include <poll.h>
#define MAX_EVENTS 50

class Server
{
  private:
    epoll_event _events[MAX_EVENTS];
    std::list<int> _clientSocketList;
    int _serverSocket;
    std::string _endpoint;
    std::list<Location*> _locationList;
    Config& _config;

    void    _sigintHandle(int signum);
    int _createServerSocket(int port);
    bool _checkLocation(const ClientRequest& clientRequest) ;

  public:
    Server();
    Server(Config& config);
    Server(const Server &src);
    ~Server();
    const Server &operator=(const Server& server);

    void printConfig(void);
    int start(void);
    void sendResponse(ClientRequest &clientRequest, int clientSocket);
    void addClientSocket(int clientSocket);
    void delClientSocket(int clientSocket);
    bool hasClientSocket(int clientSocket);
    int getServerSocket();
};
#endif
