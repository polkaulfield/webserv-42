#include "../include/pollManager.hpp"
#include <list>
#include <sys/epoll.h>
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <errno.h>

PollManager* PollManager::_instance = NULL;

Server& PollManager::_getServerByEventFd(int socket)
{
	for (std::list<Server>::iterator server = _serverList.begin(); server != _serverList.end(); ++server)
	{
		// Check here if one of the polls is a server
		if (socket == server->getServerSocket())
		{
			return *server;
		}
	}
	throw std::runtime_error("Not a server socket");
}

Server& PollManager::_getServerByClientSocket(int socket)
{
	for (std::list<Server>::iterator server = _serverList.begin(); server != _serverList.end(); ++server)
	{
		if(server->hasClientSocket(socket))
		{
			return *server;
		}
	}

	throw std::runtime_error("Not a client socket");
}

int PollManager::_initEpollWithServers(std::list<Server>& serverList)
{
	int epollFd = epoll_create1(0);
	struct epoll_event event;
	if (epollFd == -1)
	{
		std::cerr << "Failed to create epoll" << std::endl;
		return (-1);
	}

	// Start all the server listener sockets and add their fds to the pfds arr
	for (std::list<Server>::iterator server = serverList.begin(); server != serverList.end(); ++server)
	{
		event.events = EPOLLIN;
		event.data.fd = server->getServerSocket();
		// Add the server sockets to a list so we can check the fds easily later
		_serverSocketList.insert(server->getServerSocket());
		if (epoll_ctl(epollFd, EPOLL_CTL_ADD, server->getServerSocket(), &event) == -1) {
			return (-1);
		}
	}
	return (epollFd);
}

PollManager::PollManager(std::list<Server> &serverList)
{
	_serverList = serverList;
}

PollManager::~PollManager(void)
{

}

void    PollManager::_sigintHandle(int sigint)
{
	(void)sigint;
	std::cout << "CTRL+C detected! Closing all fds!" << std::endl;
	if (_instance != NULL)
	{
		for (std::list<Server>::iterator server = _instance->_serverList.begin(); server != _instance->_serverList.end(); ++server)
		{
			server->closeAllSockets();
		}
	}
	std::exit(0);
}

void PollManager::start(void)
{
	_instance = this;
	signal(SIGINT, _sigintHandle);
	struct epoll_event event, events[MAX_EVENTS];
	// Create all the epoll needed stuff.
	int epollFd = _initEpollWithServers(_serverList);
	if (epollFd == -1)
	{
		std::cerr << "Failed to add server socket to epoll instance." << std::endl;
		std::exit(1);
	}
	// Main loop
	while (true)
	{
			int numEvents = epoll_wait(epollFd, events, MAX_EVENTS, -1);
			if (numEvents == -1) {
				std::cerr << "Failed to wait for events." << std::endl;
				break;
			}
			else
			{
				// Iterate for all the events ready set by epoll
				for (int i = 0; i < numEvents; ++i) {
					int eventFd = events[i].data.fd;
					// Check if the eventFd is a socketfd in the server list
					if (_serverSocketList.count(eventFd)) {
						Server &server = _getServerByEventFd(eventFd);
						// Okay so we are the server socket. Assign this variable for clarity.
						int serverSocket = server.getServerSocket();
						// If listener is ready to read, handle new connection
						struct sockaddr_in clientAddress;
						socklen_t clientAddressLength = sizeof(clientAddress);
						int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressLength);
						if (clientSocket == -1) {
							std::cerr << "Detected clientSocket error!" << std::endl;
							perror("accept");
							continue;
						}
						std::cout << "Got a new cozennection!" << std::endl;
						event.events = EPOLLIN;
						event.data.fd = clientSocket;
						if (epoll_ctl(epollFd, EPOLL_CTL_ADD, clientSocket, &event) == -1) {
							std::cerr << "Failed to add client socket to epoll instance." << std::endl;
							close(clientSocket);
							continue;
						}
						server.addClientSocket(clientSocket);
					}
					else {
						Server& server = _getServerByClientSocket(eventFd);
						//Then the socket is the client fd
						int clientSocket = eventFd;
						// Read from the socket
						std::string fullRequest;
						char buffer[4096];
						int	totalBytesRead = 0;
						int	expectedBodyLength = -1;
						bool	headersComplete = false;

						while (true) {
							int b_read = recv(clientSocket, buffer, sizeof buffer - 1, 0);
							if (b_read <= 0) {
								// Got error or connection closed by client
								if (b_read == 0) {
									// Connection closed
									std::cout << "Connection closed!" << std::endl;
								}
								else {
									perror("recv");
									std::cout << "Socket error!" << std::endl;
								}
								break;
							}
							buffer[b_read] = '\0';
							fullRequest += buffer;
							totalBytesRead += b_read;


							// Verificar si los headers están completos
							if (!headersComplete) {
								size_t headerEnd = fullRequest.find("\r\n\r\n");
								if (headerEnd != std::string::npos) {
									headersComplete = true;

									// Extraer Content-Length
									size_t clPos = fullRequest.find("Content-Length:");
									if (clPos != std::string::npos) {
										size_t start = clPos + 15;
										size_t end = fullRequest.find("\r\n", start);
										if (end != std::string::npos)
											expectedBodyLength = std::atoi(fullRequest.substr(start, end - start).c_str());
									}
								}
							}

							// Si tenemos los headers completos, verificar si tenemos todo el body
							if (headersComplete && expectedBodyLength >= 0) {
								size_t headerEnd = fullRequest.find("\r\n\r\n");
								int currentBodyLength = fullRequest.length() - headerEnd - 4;

								if (currentBodyLength >= expectedBodyLength)
									break;
							}
							else if (headersComplete && expectedBodyLength == -1) {
								if (b_read < static_cast<int>(sizeof(buffer) - 1))
									break;
							}
						}

						if (!fullRequest.empty()) {
							// We got some good data from the browser
							// Null terminate the buffer so it doesnt get out of hand
							// Create the request we are gonna send back
							ClientRequest clientRequest = ClientRequest(const_cast<char*>(fullRequest.c_str()), server.getConfig());
							server.sendResponse(clientRequest, clientSocket);
						}
						// Remove the client socket from server and epoll
						close(clientSocket);
						server.delClientSocket(clientSocket);
						epoll_ctl(epollFd, EPOLL_CTL_DEL, clientSocket, &event);
					}
				}
			}
		}
	}
