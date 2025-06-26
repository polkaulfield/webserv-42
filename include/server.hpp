#include <string>
#include <unistd.h>

class Server
{
  private:
    int _serverSocket;
    std::string _endpoint;

    void    _sigintHandle(int signum);
    int _createServerSocket(int port);

  public:
    Server();
    Server(int port, std::string endpoint);
    Server(const Server& server);
    ~Server();
    Server &operator=(const Server& server);
    void start(void);
};
