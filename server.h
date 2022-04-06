#pragma once
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <vector>
#include <map>
#include <memory>


struct SocketClient{
    int socketFd;
    char src_ip[NI_MAXHOST];
    char src_port[NI_MAXSERV];
}__attribute__((packed));

class Server{
    public:
        Server(int _port, int _maxConnections, int _maxConnectionsQueue, int _reciveBufferSize);
        void runServer();
        int createListener();
        std::map<int, SocketClient>& clients();
        int sendMessage(int destinationFileDescriptor, std::string &message);
        int disconectClient(int clientFileDescriptor);
    private:
        int port;
        int maxConnections;
        int maxConnectionsQueue;
        int listeningSocket;
        int epollFileDescriptors;
        int reciveBufferSize;
        std::map<int,SocketClient> connectedClients;
        
        
};