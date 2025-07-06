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

bool PollManager::_isServerSocket(int socket)
{
    for (std::list<Server>::iterator server = _serverList.begin(); server != _serverList.end(); ++server)
    {
        // Check here if one of the polls is a server
        if (socket == server->getServerSocket())
            return true;
    }
    return false;
}

void PollManager::_add_to_pfds(struct pollfd *pfds[], int newfd, int *fd_count, int *fd_size)
{
    // If we don't have room, add more space in the pfds array
    if (*fd_count == *fd_size) {
        *fd_size *= 2; // Double it

        *pfds = (struct pollfd *)realloc(*pfds, sizeof(**pfds) * (*fd_size));
    }

    (*pfds)[*fd_count].fd = newfd;
    (*pfds)[*fd_count].events = POLLIN; // Check ready-to-read

    (*fd_count)++;
}

// Remove an index from the set
void PollManager::_del_from_pfds(struct pollfd pfds[], int i, int *fd_count)
{
    // Copy the one from the end over this one
    pfds[i] = pfds[*fd_count-1];

    (*fd_count)--;
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
            {
                for (int i = 0; i < numEvents; ++i) {
                    int event_fd = events[i].data.fd;
                    if(_isServerSocket(event_fd))
                    {
                        // If listener is ready to read, handle new connection
                        struct sockaddr_in clientAddress;
                        socklen_t clientAddressLength = sizeof(clientAddress);
                        clientSocket = accept(event_fd, (struct sockaddr*)&clientAddress, &clientAddressLength);
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
                    }
                    else {
                        clientSocket = event_fd;
                        b_read = recv(clientSocket, buffer, sizeof buffer - 1, 0);
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
                            clientRequest = ClientRequest(buffer);
                            _serverList.front().sendResponse(clientRequest, clientSocket);
                        }
                        close(clientSocket);
                        epoll_ctl(epollFd, EPOLL_CTL_DEL, clientSocket, &event);
                    }
                }
            } // END handle data from client
        } // END got ready-to-read from poll()
    } // END looping through file descriptors
