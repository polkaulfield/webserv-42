#include <csignal>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <wait.h>
#include <signal.h>
#include <string.h>
#include <cstdlib>
#include <sys/stat.h>
#include "../include/clientRequest.hpp"
#include "../include/serverResponse.hpp"
#define PORT 8080

int serverSocket = 0;

// This creates a socket listening on PORT. From it we create the clientSocket to handle the connections
int createServerSocket(int port)
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

// This is to close the server socket on ctrl+c
void    sigintHandle(int signum)
{
    (void)signum;
    std::cout << "Closing socket!" << std::endl;
    close(serverSocket);
    std::exit(0);
}

int main(int argc, char **argv)
{
    (void)argv;
    (void)argc;
    // We have to initialize the server socket first
    serverSocket = createServerSocket(PORT);
    signal(SIGINT, sigintHandle);
    if (!serverSocket)
    {
        std::cout << "Socket creation failed!" << std::endl;
        std::exit(0);
    }
    char petition_buf[1024] = {0};
    int clientSocket;
    do
    {
        // We need to create a client socket for each connection
        // using the bound socket to PORT we created before (serverSocket)
        clientSocket = accept(serverSocket, NULL, NULL);
        // This gets the data clientSocket listened to petition_buf
        recv(clientSocket, petition_buf, sizeof(petition_buf), 0);
        // Parse client request
        ClientRequest clientRequest = ClientRequest(petition_buf);
        // Create server response
        ServerResponse serverResponse = ServerResponse(clientRequest.getMethod(), clientRequest.getPath());
        send(clientSocket, serverResponse.getResponse().data(), serverResponse.getResponse().length(), 0);
        // We need to close after sending
        close(clientSocket);
    } while (clientSocket);
    return 0;
}
