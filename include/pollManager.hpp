#ifndef POLLMANAGER_HPP
# define POLLMANAGER_HPP
# include "server.hpp"
# include <string>
# include <unistd.h>
# include <list>
# include <poll.h>

class PollManager
{
  private:
    std::list<Server> _serverList;
    struct epoll_event _events[MAX_EVENTS];
    int _initEpollWithServers(std::list<Server>& serverList);
    Server& _getServerByEventFd(int socket);
    Server& _getServerByClientSocket(int socket);
  public:
    PollManager();
    PollManager(std::list<Server> &serverList);
    ~PollManager();
    void start(void);

};
#endif
