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
    // This is EXTREMELY important. This tells the kernel it doesnt have to lock PORT
    // so if the serverSocket is closed we can reuse it right away. If this line is missing
    // we have to wait 2~ min until the system frees the port.
    int yes = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    // Attach the socket to the port and listen
    if(bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1)
        std::exit(1);
    listen(serverSocket, 5);
    return serverSocket;
}

Server::Server(Config& config) : _config(config)
{
	config.printConfig();
    _locationList = config.getLocationList();
    //std::cout << "Location: " << _locationList->location->getDirectory() << " " << _locationList << std::endl;
    _serverSocket = _createServerSocket(config.getPort());
    if (!_serverSocket)
    {
        std::cout << "Socket creation failed!" << std::endl;
        std::exit(1);
    }
}

Server::Server(int port, const std::string& endpoint , Config& config) : _config(config)
{
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
    /*std::cout << "path: " << clientRequest.getPath() << std::endl;
    for (std::list<Location*>::iterator iter = _locationList.begin(); iter != _locationList.end(); ++iter)
    {
        std::cout << "Checking locations"<< std::endl;
        //std::cout << startsWith("/" + clientRequest.getPath(), (*iter)->getDirectory()) << std::endl;
        //std::cout << (*iter)->hasMethod(clientRequest.getMethod()) << std::endl;
        if ((*iter)->hasMethod(clientRequest.getMethod()) && startsWith("/" + clientRequest.getPath(), (*iter)->getDirectory()))
            return true;
    }
    return false;*/
    std::string root = "/" + clientRequest.getPath();
    int	end = 0;
    std::string tmp = root;
    bool value_return = false;
    end = tmp.find_last_of("/");
    tmp = tmp.substr(0, end + 1);
    while (!value_return && end != -1) {
	    end = tmp.find_last_of("/");
	    tmp = tmp.substr(0, end + 1);
      	if (_config.searchLocation(tmp) && _config.searchLocation(tmp)->hasMethod(clientRequest.getMethod()))
       		value_return = true;
        end = tmp.find_last_of("/");
        tmp = tmp.substr(0, end);
    }
    return value_return;
}

Server::~Server(void)
{
    close(_serverSocket);
}

int Server::getServerSocket(void)
{
    return _serverSocket;
}

void Server::sendResponse(ClientRequest clientRequest, int clientSocket)
{
    ServerResponse serverResponse;
    std::cout << "Parsing client request" << std::endl;

    // If theres no locationlist all paths and methods are valid for now (debug)
    if (_checkLocation(clientRequest) != 0)
    {
        std::cout << "Ok" << std::endl;
        serverResponse = ServerResponse(clientRequest, _config);
        send(clientSocket, serverResponse.getResponse().data(), serverResponse.getResponse().length(), 0);
    }
    else
    {
        std::cout << "Failed to find valid endpoint" << std::endl;
        send(clientSocket, ServerResponse().buildNotFoundResponse().data(), 0, 0);
    }
    close(clientSocket);
}
