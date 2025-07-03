#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <wait.h>
#include <sys/stat.h>
#include <cstdlib>
#include <list>
#include <iostream>
#include "../include/serverResponse.hpp"
#include "../include/server.hpp"
#include "../include/config.hpp"

#define PORT 8080

int main(int argc, char **argv)
{
    (void)argv;
    (void)argc;
    if (argc != 2)
        return 1;
    std::list<Config> configList = takeConfig(argv[1]);
    configList.front().printConfig();
   	Server server = Server(configList.front());
   	configList.front().printConfig();
   	server.start();

    return 0;
}
