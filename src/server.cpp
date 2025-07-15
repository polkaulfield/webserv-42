#include "../include/server.hpp"
#include "../include/serverResponse.hpp"
#include "../include/config.hpp"
#include "../include/utils.hpp"
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
#include <unistd.h>
#include <list>
#include <cstdio>
#define MAX_CLIENTS 50

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
		perror("bind");
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
	//config.printConfig();
	_config = config;
	_locationList = config.getLocationList();
	_serverSocket = 0;
	_serverSocket = _createServerSocket(config.getPort());
}

Server::Server(const Server &src) : _config(src._config) {
	//std::cout << "Server Copy constructor" << std::endl;
	*this = src;
}

const Server& Server::operator=(const Server& server)
{
	//std::cout << "Server asign operator" << std::endl;
	if (this != &server) {
		for (int i = 0; i < MAX_EVENTS; i++) {
			_events[i] = server._events[i];
			_events[i].data.fd = server._events[i].data.fd;
		}
		_serverSocket = dup(server._serverSocket);
		_locationList = server._locationList;
		_config = server._config;
		_endpoint = server._endpoint;
	}
	//std::cout << _serverSocket << " " << server._serverSocket << std::endl;
	//_config.printConfig();
	return *this;
}

bool Server::_checkLocation(const ClientRequest& clientRequest)
{
    _isFileUpload = false;

	for (std::list<Location>::iterator iter = _locationList.begin(); iter != _locationList.end(); ++iter)
	{
		std::cout << "Checking locations"<< std::endl;

		std::cout << startsWith( clientRequest.getPath(), _config.getRoot() + iter->getDirectory()) << std::endl;
		std::cout << iter->hasMethod(clientRequest.getMethod()) << std::endl;

		if (iter->hasMethod(clientRequest.getMethod()) && startsWith(clientRequest.getPath(), _config.getRoot() + iter->getDirectory()))
		{
		    // Workaround to tell the server if we are uploading
		    if (iter->getIsUpload())
			{
				_isFileUpload = true;
				std::cout << "We are uploading files!" << std::endl;
				return true;
			}
			if (access(clientRequest.getPath().c_str(), F_OK) != -1)
				return true;
		}
	}
	return false;
}

void Server::closeAllSockets(void)
{
    for (std::list<int>::iterator clientSocket = _clientSocketList.begin(); clientSocket != _clientSocketList.end(); ++clientSocket)
    {
        std::cout << "Closing client socket!" << std::endl;
        close(*clientSocket);
    }
    close(_serverSocket);
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
		serverResponse = ServerResponse(clientRequest, _config, _isFileUpload);
		send(clientSocket, serverResponse.getResponse().data(), serverResponse.getResponse().length(), 0);
	}
	else
	{
		std::cout << "Failed to find valid endpoint" << std::endl;
		send(clientSocket, ServerResponse().buildNotFoundResponse().data(), ServerResponse().buildNotFoundResponse().length(), 0);
	}
	close(clientSocket);
}

const Config& Server::getConfig(void) const
{
	return _config;
}

bool Server::isUploading() const
{
    return _isFileUpload;
}

void Server::printConfig(void) {_config.printConfig();}
