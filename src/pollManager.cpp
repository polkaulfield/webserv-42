#include "../include/pollManager.hpp"
#include <cstddef>
#include <list>
#include <sys/epoll.h>
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <bits/stdc++.h>
#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>

Server& PollManager::_getServerByEventFd(int socket, bool *found)
{
    for (std::list<Server>::iterator server = _serverList.begin(); server != _serverList.end(); ++server)
    {
        // Check here if one of the polls is a server
        if (socket == server->getServerSocket())
        {
            *found = true;
            return *server;
        }
    }
    *found = false;
    return _serverList.front();
}

Server& PollManager::_getServerByClientSocket(int socket, bool *found)
{
    for (std::list<Server>::iterator server = _serverList.begin(); server != _serverList.end(); ++server)
    {
        if(server->hasClientSocket(socket))
        {
            *found = true;
            return *server;
        }
    }
    *found = false;
    return _serverList.front();
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
    int b_read;
    char buffer[1024];
    int clientSocket;
    int serverSocket;
    ClientRequest clientRequest;
    int epollFd;
    struct epoll_event event, events[MAX_EVENTS];

    epollFd = epoll_create1(0);
    if (epollFd == -1)
    {
        std::cerr << "Failed to create epoll" << std::endl;
        std::exit(1);
    }

    // Start all the server listener sockets and add their fds to the pfds arr
    for (std::list<Server>::iterator server = _serverList.begin(); server != _serverList.end(); ++server)
    {
        event.events = EPOLLIN;
        event.data.fd = server->getServerSocket();
        if (epoll_ctl(epollFd, EPOLL_CTL_ADD, server->getServerSocket(), &event) == -1) {
            std::cerr << "Failed to add server socket to epoll instance." << std::endl;
            std::exit(1);
        }
    }
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
                    bool isServer = false;
                    Server &server = _getServerByEventFd(eventFd, &isServer);
                    if (isServer)
                    {
                        // Okay so we are the server socket. Assign this variable for clarity.
                        serverSocket = server.getServerSocket();
                        // If listener is ready to read, handle new connection
                        struct sockaddr_in clientAddress;
                        socklen_t clientAddressLength = sizeof(clientAddress);
                        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressLength);
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
                    else {
                        // Not a server. We assume its client
                        bool found = false;
                        Server& server = _getServerByClientSocket(eventFd, &found);
                        // Check if the client is associated to a server for sanity
                        if (found)
                        {
                            //Then the socket is the client fd
                            clientSocket = eventFd;
                            // Read from da socket
                            b_read = recv(clientSocket, buffer, sizeof buffer - 1, 0);
                            if (b_read <= 0) {
                                // Got error or connection closed by client
                                if (b_read == 0) {
                                    // Connection closed7070
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
                                clientRequest = ClientRequest(buffer);
                                _serverList.front().sendResponse(clientRequest, clientSocket);
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
    }
