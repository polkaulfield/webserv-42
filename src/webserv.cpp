#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <wait.h>
#include <sys/stat.h>
#include "../include/clientRequest.hpp"
#include "../include/serverResponse.hpp"
#include "../include/server.hpp"
#define PORT 8080

int main(int argc, char **argv)
{
    (void)argv;
    (void)argc;

    Server server = Server(PORT, "/");
    server.start();

    return 0;
}
