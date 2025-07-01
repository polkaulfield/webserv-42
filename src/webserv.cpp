#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <wait.h>
#include <sys/stat.h>
#include "../include/serverResponse.hpp"
#include "../include/server.hpp"
#include "../include/config.hpp"

#define PORT 9090

int main(int argc, char **argv)
{
    (void)argv;
    if (argc != 2)
        return 1;
    Config *config = takeConfig(argv[1]);
    Server server = Server(config[0]);
    config[0].printConfig();
    server.start();

    return 0;
}
