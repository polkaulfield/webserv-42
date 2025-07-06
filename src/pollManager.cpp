#include "../include/pollManager.hpp"
#include <csignal>
#include <cstddef>
#include <exception>
#include <list>
#include <sys/epoll.h>
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <bits/stdc++.h>
#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>

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

void PollManager::start(void)
{
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
                for (int i = 0; i < numEvents; ++i) {
                    int eventFd = events[i].data.fd;
                    // Check if the event raised by poll is a server socket
                    try {
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
                        std::cout << "Got a new connection!" << std::endl;
                        event.events = EPOLLIN;
                        event.data.fd = clientSocket;
                        if (epoll_ctl(epollFd, EPOLL_CTL_ADD, clientSocket, &event) == -1) {
                            std::cerr << "Failed to add client socket to epoll instance." << std::endl;
                            close(clientSocket);
                            continue;
                        }
                        server.addClientSocket(clientSocket);
                    }
                    catch (const std::exception& e)
                    {
                        // Check if the exception indicates the socket fd wasnt a server
                        // We assume it's a client
                        if (strcmp(e.what(), "Not a server socket") == 0)
                        {
                            // Check if the client is associated to a server for sanity
                            try {
                                Server& server = _getServerByClientSocket(eventFd);
                                //Then the socket is the client fd
                                int clientSocket = eventFd;
                                // Read from the socket
                                char buffer[1024];
                                int b_read = recv(clientSocket, buffer, sizeof buffer - 1, 0);
                                if (b_read <= 0) {
                                    // Got error or connection closed by client
                                    if (b_read == 0) {
                                        // Connection closed
                                        std::cout << "Connection closed!" << std::endl;
                                    } else {
                                        perror("recv");
                                        std::cout << "Socket error!" << std::endl;
                                    }
                                }
                                else
                                {
                                    // We got some good data from the browser
                                    // Null terminate the buffer so it doesnt get out of hand
                                    buffer[b_read] = '\0';
                                    // Create the request we are gonna send back
                                    ClientRequest clientRequest = ClientRequest(buffer);
                                    _serverList.front().sendResponse(clientRequest, clientSocket);
                                }
                                // Remove the client socket from server and epoll
                                close(clientSocket);
                                server.delClientSocket(clientSocket);
                                epoll_ctl(epollFd, EPOLL_CTL_DEL, clientSocket, &event);
                            }
                            catch (const std::exception& e)
                            {
                                std::cerr << "Exception: " << e.what() << std::endl;
                            }
                        }
                    }
                }
            }
        }
    }
