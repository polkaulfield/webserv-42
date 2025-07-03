#ifndef SERVER_HPP
# define SERVER_HPP
#include <string>
#include <unistd.h>
#include "clientRequest.hpp"
#include "location.hpp"
#include "config.hpp"
#include <list>
#include <poll.h>

class Server
{
  private:
    int _serverSocket;
    std::string _endpoint;
    std::list<Location*> _locationList;
    Config& _config;

    void    _sigintHandle(int signum);
    int _createServerSocket(int port);
    bool _checkLocation(const ClientRequest& clientRequest) ;

  public:
    Server();
    Server(int port, const std::string& endpoint, Config& config);
    Server(Config& config);
    Server(const Server& server);
    ~Server();
    Server &operator=(const Server& server);

    int start(void);
    void sendResponse(ClientRequest clientRequest, int clientSocket);
    int getServerSocket();
};
#endif
