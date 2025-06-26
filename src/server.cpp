#include "../include/server.hpp"
#include "../include/clientRequest.hpp"
#include "../include/serverResponse.hpp"
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <wait.h>
#include <string.h>
#include <cstdlib>
#include <sys/stat.h>
#include <sys/stat.h>

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

Server::Server(int port, std::string endpoint)
{
    _endpoint = endpoint;
    _serverSocket = _createServerSocket(port);
    if (!_serverSocket)
    {
        std::cout << "Socket creation failed!" << std::endl;
        std::exit(0);
    }
}

void Server::start()
{
    char buf[1024];
    int clientSocket;
    int b_read = 0;
    while (true)
    {
        // We need to create a client socket for each connection
        // using the bound socket to PORT we created before (serverSocket)
        clientSocket = accept(_serverSocket, NULL, NULL);
        if (clientSocket == -1)
            std::cout << "Detected clientSocket error!" << std::endl;
        // This gets the data clientSocket listened to petition_buf
        while (b_read != -1)
            b_read = recv(clientSocket, buf, sizeof(buf), 0);
        if (b_read == -1 )
        {
            std::cout << "Socket error!" << std::endl;
            std::exit(0);
        }
        buf[b_read] = '\0';
        // Parse client request
        ClientRequest clientRequest = ClientRequest(buf);
        // Create server response
        ServerResponse serverResponse = ServerResponse(clientRequest.getMethod(), clientRequest.getPath());
        send(clientSocket, serverResponse.getResponse().data(), serverResponse.getResponse().length(), 0);
        // We need to close after sending
        close(clientSocket);
    }
    return ;
}

Server::~Server(void)
{
    close(_serverSocket);
}
