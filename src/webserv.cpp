#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <wait.h>
#include <sys/stat.h>
#include <list>
#include <iostream>
#include "../include/serverResponse.hpp"
#include "../include/server.hpp"
#include "../include/config.hpp"
#include "../include/pollManager.hpp"
#include <poll.h>
#include <list>

int main(int argc, char **argv)
{
	std::list<Config> configList;
    if (argc != 2)
        configList = takeConfig("configs/default.conf");
    else
        configList = takeConfig(argv[1]);
    std::list<Server> serverList;
    for (std::list<Config>::iterator config = configList.begin(); config != configList.end(); ++config)
    {
        serverList.push_back(Server(*config));
    }
    PollManager pollManager(serverList);
    pollManager.start();
    return 0;
}
