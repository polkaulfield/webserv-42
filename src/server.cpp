#include "../include/server.hpp"
#include "../include/serverResponse.hpp"
#include "../include/config.hpp"
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <string>
#include <unistd.h>
#include <wait.h>
#include <cstdlib>
#include <algorithm>
#include <sys/stat.h>
#include <list>
#define MAX_CLIENTS 50

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
    if (_serverSocket == -1)
    {
         std::cerr << "Failed to create socket" << std::endl;
         std::exit(1);
    }
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    // This is EXTREMELY important. This tells the kernel it doesnt have to lock PORT
    // so if the serverSocket is closed we can reuse it right away. If this line is missing
    // we have to wait 2~ min until the system frees the port.
    int yes = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    // Attach the socket to the port and listen
    if(bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1)
    {
        std::cerr << "Failed to bind socket" << std::endl;
        close(serverSocket);
        std::exit(1);
    }
    if (listen(serverSocket, MAX_CLIENTS) == -1) {
        std::cerr << "Failed to listen." << std::endl;
        close(serverSocket);
        std::exit(1);
    }
    return serverSocket;
}

Server::Server(Config& config) : _config(config)
{
	config.printConfig();
    _locationList = config.getLocationList();
    _serverSocket = _createServerSocket(config.getPort());
}

Server::Server(const Server &src) : _config(src._config) {
	std::cout << "Server Copy constructor" << std::endl;
	*this = src;
}

const Server& Server::operator=(const Server& server)
{
	std::cout << "Server asign operator" << std::endl;
	if (this != &server) {
		for (int i = 0; i < MAX_EVENTS; i++) {
			_events[i] = server._events[i];
			_events[i].data.fd = dup(server._events[i].data.fd);
		}
		_serverSocket = dup(server._serverSocket);
	    _locationList = server._locationList;
	    _config = server._config;
	    _endpoint = server._endpoint;
	}
    std::cout << _serverSocket << " " << server._serverSocket << std::endl;
    //_config.printConfig();
    return *this;
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

void Server::addClientSocket(int clientSocket)
{
    _clientSocketList.push_back(clientSocket);
}

void Server::delClientSocket(int clientSocket)
{
    _clientSocketList.remove(clientSocket);
}

bool Server::hasClientSocket(int clientSocket)
{
    std::list<int>::iterator pos;
    pos = find(_clientSocketList.begin(), _clientSocketList.end(), clientSocket);
    if (pos != _clientSocketList.end())
        return true;
    return false;
}

void Server::sendResponse(ClientRequest &clientRequest, int clientSocket)
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

void Server::printConfig(void) {_config.printConfig();}
