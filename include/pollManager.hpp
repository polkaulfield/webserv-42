#ifndef POLLMANAGER_HPP
# define POLLMANAGER_HPP
# include "server.hpp"
# include <unistd.h>
# include <list>
# include <poll.h>
# include <set>

class PollManager
{
  private:
    std::list<Server> _serverList;
    std::set<int> _serverSocketList;
    struct epoll_event _events[MAX_EVENTS];
    void _iterateEpollEvents(int epollFd, struct epoll_event *events, int numEvents);
    int _initEpollWithServers(std::list<Server>& serverList);
    Server& _getServerByEventFd(int socket);
    Server& _getServerByClientSocket(int socket);
    static PollManager* _instance;
    static void _sigintHandle(int signum);
  public:
    PollManager();
    PollManager(std::list<Server> &serverList);
    ~PollManager();
    void start(void);

};
#endif
