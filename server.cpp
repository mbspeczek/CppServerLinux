#include "server.h"

Server::Server(int _port, int _maxConnections, int _maxConnectionsQueue, int _reciveBufferSize)
: port(_port)
, maxConnections(_maxConnections)
, maxConnectionsQueue(_maxConnectionsQueue)
, reciveBufferSize(_reciveBufferSize)
{}


int Server::disconectClient(int clientFileDescriptor){
    if( connectedClients.find(clientFileDescriptor)->first == clientFileDescriptor ){
        epoll_ctl(epollFileDescriptors, clientFileDescriptor, EPOLL_CTL_DEL,0);
        close(clientFileDescriptor);
        connectedClients.erase(clientFileDescriptor);
        
        return 1;
    }
    
    return 0;
}

int Server::createListener(){
    
    listeningSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(listeningSocket == -1){
        perror("Cant create socket");
        exit(0);
    }

    sockaddr_in listeningAddress;
    socklen_t listeningAddressSize = sizeof(listeningAddress);
    memset(&listeningAddress, 0, sizeof(listeningAddress));

    listeningAddress.sin_family = AF_INET;
    listeningAddress.sin_addr.s_addr = INADDR_ANY;
    listeningAddress.sin_port = htons(port);
    
    if(bind(listeningSocket, (sockaddr*)&listeningAddress, listeningAddressSize) < 0){
        perror("Cant bind Listener socket");
        exit(1);
    }
    if (listen(listeningSocket,maxConnectionsQueue) == -1)
    {
        perror("Cant listen");
        exit(2);
    }
    
    return 1;

}

std::map<int, SocketClient>& Server::clients(){
    return connectedClients;
}

int Server::sendMessage(int destinationFileDescriptor, std::string &message){
    size_t size = message.size();
    if(size>0){
        return send(destinationFileDescriptor,message.data(),size,0);
    }
    return -1;
}

void Server::runServer(){
    int triggeredFileDescriptorsCount = 0;
    int currentFileDescriptor = 0;
    int currentConnectionSocket = 0;
    int recivedBytes = 0;
    epoll_event ev, events[100];
    std::shared_ptr<char> buffer = std::make_shared<char>(reciveBufferSize);

    if( createListener() !=1 ){
        perror("cant run server");
        exit(3);        
    }
    

    epollFileDescriptors = epoll_create1(0);
    if( epollFileDescriptors == -1 ){
        perror("cant create epoll");
        exit(4);
    }

    ev.events = EPOLLIN;
    ev.data.fd = listeningSocket;
    if( epoll_ctl(epollFileDescriptors, EPOLL_CTL_ADD, listeningSocket, &ev) == -1){
        perror("cant add listener socket to epoll");
        exit(5);
    }
    
    for(;;){
        triggeredFileDescriptorsCount = epoll_wait(epollFileDescriptors,events,sizeof(events),200);
        if( triggeredFileDescriptorsCount == -1){
            perror("epoll wait cant work");
            exit(6);
        }

        for( int n = 0; n<triggeredFileDescriptorsCount; n++){
            currentFileDescriptor = events[n].data.fd;

            if(currentFileDescriptor == listeningSocket){
                sockaddr_in clientAddress;
                socklen_t clientSize = sizeof(clientAddress);
                
                currentConnectionSocket = accept4(listeningSocket, (sockaddr*)&clientAddress, &clientSize, SOCK_NONBLOCK );
                if( currentConnectionSocket == -1){
                    perror("cant accept new connection");
                    continue;
                }
                else{
                    SocketClient client;
                    char host[NI_MAXHOST];
                    char service[NI_MAXSERV];
                    memset(host, 0, NI_MAXHOST);
                    memset(service, 0, NI_MAXSERV);

                    if(getnameinfo((sockaddr*)&clientAddress, sizeof(clientAddress), host, NI_MAXHOST, service, NI_MAXSERV,0)){
                        client.socketFd = currentConnectionSocket;
                        memcpy(client.src_ip,host, strlen(host)+1);
                        memcpy(client.src_port,service, strlen(service)+1);
                        connectedClients.insert(std::pair<int,SocketClient>(currentConnectionSocket,client) );
                    }
                    else{
                        inet_ntop(AF_INET, &clientAddress.sin_addr, host, NI_MAXHOST);
                        client.socketFd = currentConnectionSocket;
                        memcpy(client.src_ip,host, strlen(host)+1);
                        memcpy(client.src_port,service, strlen(service)+1);
                        connectedClients.insert(std::pair<int,SocketClient>(currentConnectionSocket,client) );
                    }

                    ev.events = EPOLLIN | EPOLLET;
                    ev.data.fd = currentConnectionSocket;
                    if (epoll_ctl(epollFileDescriptors, EPOLL_CTL_ADD, currentConnectionSocket, &ev) == -1) {
                            perror("epoll_ctl: conn_sock");
                            exit(EXIT_FAILURE);
                    }
                }

            }
            else {
                recivedBytes = recv(currentFileDescriptor,&buffer, sizeof(buffer),0);
                if(recivedBytes == -1){
                    std::cout<<"Error in reciving."<<std::endl;
                    
                    epoll_ctl(epollFileDescriptors, currentFileDescriptor, EPOLL_CTL_DEL,0);
                    close(currentFileDescriptor);
                    connectedClients.erase(currentFileDescriptor);
                    
                    continue;
                }
                if(recivedBytes == 0){
                    std::cout<<"Client disconected."<<std::endl;
                    epoll_ctl(epollFileDescriptors, currentFileDescriptor, EPOLL_CTL_DEL,0);
                    close(currentFileDescriptor);
                    connectedClients.erase(currentFileDescriptor);
                    continue;
                }

                std::cout<<"Recived bytes from "<< connectedClients.find(currentFileDescriptor)->second.src_ip<<std::endl;

            }

        }

    }

}