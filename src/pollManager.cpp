#include "../include/pollManager.hpp"
#include <cstddef>
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <bits/stdc++.h>
#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>

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

PollManager::PollManager(void)
{

}

PollManager::PollManager(std::list<Server> serverList)
{
    _serverList = serverList;
}

PollManager::~PollManager(void)
{

}

void PollManager::start(void)
{
    int fd_count = 0;
    int fd_size = 5;
    int b_read;
    char buffer[1024];
    int clientSocket;
    ClientRequest clientRequest;
    struct pollfd *pfds = (struct pollfd*)malloc(sizeof *pfds * fd_size);

    // Start all the server listener sockets and add their fds to the pfds arr
    for (std::list<Server>::iterator server = _serverList.begin(); server != _serverList.end(); ++server)
    {
        _add_to_pfds(&pfds, server->getServerSocket(), &fd_count, &fd_size);
    }
    for(int i = 0; i < fd_count; i++) {
        // Check if someone's ready to read
        if (pfds[i].revents & POLLIN) { // We got one!!
            for (std::list<Server>::iterator server = _serverList.begin(); server != _serverList.end(); ++server)
            {
                // Check here if one of the polls is a server
                if (pfds[i].fd == server->getServerSocket()) {
                    // If listener is ready to read, handle new connection
                    clientSocket = accept(server->getServerSocket(), NULL, NULL);

                    if (clientSocket == -1) {
                        std::cout << "Detected clientSocket error!" << std::endl;
                    } else {
                        std::cout << "Got a new connection!" << std::endl;
                        _add_to_pfds(&pfds, clientSocket, &fd_count, &fd_size);
                    }
                } else {
                    // If not the listener, we're just a regular client
                    b_read = recv(pfds[i].fd, buffer, sizeof buffer, 0);

                    if (b_read <= 0) {
                        // Got error or connection closed by client
                        if (b_read == 0) {
                            // Connection closed
                            std::cout << "Connection closed!" << std::endl;
                        } else {
                            std::cout << "Socket error!" << std::endl;
                        }
                        close(pfds[i].fd); // Bye!
                        _del_from_pfds(pfds, i, &fd_count);
                    }
                    else {
                        // We got some good data from the browser
                        // Null terminate the buffer so it doesnt get out of hand
                        buffer[b_read] = '\0';
                        // Create the request we are gonna send back
                        clientRequest = ClientRequest(buffer);
                        server->sendResponse(clientRequest, clientSocket);
                    }
                }
            } // END handle data from client
        } // END got ready-to-read from poll()
    } // END looping through file descriptors
} // END for(;;)--and you thought it would never end!
