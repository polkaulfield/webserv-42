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
    struct pollfd *pfds;
    std::list<Server> _serverList;
    std::string _getBody(std::string request);
    void _add_to_pfds(struct pollfd *pfds[], int newfd, int *fd_count, int *fd_size);
    void _del_from_pfds(struct pollfd pfds[], int i, int *fd_count);
  public:
    PollManager();
    PollManager(std::list<Server> serverList);
    ~PollManager();


    void start(void);

};
#endif
