#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <wait.h>
#include <sys/stat.h>
#include "../include/serverResponse.hpp"
#include "../include/server.hpp"
#include "../include/Config.hpp"

#include <iostream>

#define PORT 8080

int main(int argc, char **argv)
{
    (void)argv;
    (void)argc;
    Config *config = takeConfig(argv[1]);
    Server server = Server(config[0]);
    config[0].printConfig();
    server.start();

    return 0;
}
