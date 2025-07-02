#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <wait.h>
#include <sys/stat.h>
#include <iostream>
#include "../include/serverResponse.hpp"
#include "../include/server.hpp"
#include "../include/config.hpp"
#include <poll.h>
#include <list>

#define PORT 9090

int listener()
{
    char buf[1024];
    int clientSocket;
    int b_read = 0;
    ServerResponse serverResponse;
    ClientRequest clientRequest;
    std::cout << "Starting listen loop" << std::endl;
    while (true)
    {
        // We need to create a client socket for each connection
        // using the bound socket to PORT we created before (serverSocket)
        clientSocket = accept(_serverSocket, NULL, NULL);
        std::cout << "Got connection" << std::endl;
        if (clientSocket == -1)
            std::cout << "Detected clientSocket error!" << std::endl;
        // This gets the data clientSocket listened to petition_buf
        //while (b_read != -1)
        b_read = recv(clientSocket, buf, sizeof(buf), 0);
        if (b_read == -1 )
        {
            std::cout << "Socket error!" << std::endl;
            std::exit(0);
        }
        buf[b_read] = '\0';
        // Parse client request
        std::cout << "Parsing client request" << std::endl;
        clientRequest = ClientRequest(buf);
        // If theres no locationlist all paths and methods are valid for now (debug)
        if (_checkLocation(clientRequest) != 0)
        {
            std::cout << "Ok" << std::endl;
            serverResponse = ServerResponse(clientRequest, _config);
            send(clientSocket, serverResponse.getResponse().data(), serverResponse.getResponse().length(), 0);
        }
        else
        {
            std::cout << "Failed" << std::endl;
            //send(clientSocket, ServerResponse().buildNotFoundResponse().data(), 0, 0);
        }
        // We need to close after sending
        close(clientSocket);
    }
}

void add_to_pfds(std::list<struct pollfd> pfdList, int newfd)
{
    struct pollfd pfd;
    pfd.fd = newfd;
    pfd.events = POLLIN;
    pfdList.push_back(pfd);
}

int main(int argc, char **argv)
{
    (void)argv;
    if (argc != 2)
        return 1;
    std::list<Server> serverList;
    for (std::list<Config>::iterator config = configList.begin(); config != configList.end(); ++config)
    {
        serverList.push_back(Server(config));
    }
    for (std::list<Server>::iterator server = serverList.begin(); server != serverList.end(); ++server)
    {

    }
    //Config *config = takeConfig(argv[1]);
    //Server server = Server(config[0]);
    //config[0].printConfig();
    //server.start();


    return 0;
}
