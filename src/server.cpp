#include "../include/server.hpp"
#include "../include/serverResponse.hpp"
#include "../include/config.hpp"
#include "../include/utils.hpp"
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <wait.h>
#include <string.h>
#include <cstdlib>
#include <sys/stat.h>
#include <list>

// This is to close the server socket on ctrl+c
void    Server::_sigintHandle(int signum)
{
    (void)signum;
    std::cout << "Closing socket!" << std::endl;
    close(_serverSocket);
    std::exit(0);
}

// This creates a socket listening on PORT. From it we create the clientSocket to handle the connections
int Server::_createServerSocket(int port)
{
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    int optval = 1;
    // This is EXTREMELY important. This tells the kernel it doesnt have to lock PORT
    // so if the serverSocket is closed we can reuse it right away. If this line is missing
    // we have to wait 2~ min until the system frees the port.
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    // Attach the socket to the port and listen
    if(bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1)
        std::exit(1);
    listen(serverSocket, 5);
    return serverSocket;
}

Server::Server(const Config& config) : _config(config)
{
	config.printConfig();
    _locationList = config.getFirstLocation();
    std::cout << "Location: " << _locationList->location->getDirectory() << " " << _locationList << std::endl;
    _serverSocket = _createServerSocket(config.getPort());
    if (!_serverSocket)
    {
        std::cout << "Socket creation failed!" << std::endl;
        std::exit(1);
    }
}

Server::Server(int port, const std::string& endpoint , const Config& config) : _config(config)
{
    _locationList = NULL;
    _endpoint = endpoint;
    _serverSocket = _createServerSocket(port);
    if (!_serverSocket)
    {
        std::cout << "Socket creation failed!" << std::endl;
        std::exit(1);
    }
    std::cout << "Socket creation succeded" << std::endl;
}

bool Server::_checkLocation(const ClientRequest& clientRequest)
{
    location_t* location_iter = _locationList;
    std::cout << "path: " << clientRequest.getPath() << std::endl;
    while (location_iter)
    {
        std::cout << "Checking locations"<< std::endl;
        Location *location = location_iter->location;
        std::cout << startsWith("/" + clientRequest.getPath(), location->getDirectory()) << std::endl;
        if (location->hasMethod(clientRequest.getMethod()) && startsWith("/" + clientRequest.getPath(), location->getDirectory()))
            return true;
        location_iter = location_iter->next;
    }
    return false;
}

// TODO this https://beej.us/guide/bgnet/examples/pollserver.c
void Server::start()
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
    return ;
}

Server::~Server(void)
{
    close(_serverSocket);
}
int b_read = 0;
