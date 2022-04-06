#include <iostream>

#include <thread>

#include "server.h"
#include "parsedMessage.h"
#include "executeCommand.h"

// main thread - server
// 1st thread - connection



void serverService(Server &server){
        server.runServer();
}
int main(void){
    Server server(50050, 10, 10, 512);
    CommandExec commandExecutor;
    std::thread serverServiceThread(serverService, std::ref(server));
    serverServiceThread.detach();
    std::string command;

    // command destination message(string)
    commandExecutor.registerCommand("sendTo",[&](ParsedMessage msg){
        if(msg.returnParsedMessage().size() == 3){
            int fd = std::atoi(msg.returnParsedMessage().at(1).c_str());
            std::string message{msg.returnParsedMessage().at(2).c_str()};
            server.sendMessage(fd, message);
        }
    });

    commandExecutor.registerCommand("who",[&](ParsedMessage msg){
        if(msg.returnParsedMessage().size() == 1){
            for(auto& i: server.clients()){
                std::cout<<i.first<<" - "<<i.second.src_ip<<":"<<i.second.src_port<<std::endl;
            }
        }
    });

    commandExecutor.registerCommand("kick",[&](ParsedMessage msg){
        if(msg.returnParsedMessage().size() == 2){
            int kickedFileDescriptor = std::atoi(msg.returnParsedMessage().at(1).c_str());
            if( kickedFileDescriptor >=0 && server.disconectClient(kickedFileDescriptor)){
                std::cout<<"Client with fd: "<<kickedFileDescriptor<<" kicked."<<std::endl;
            }
            else{
                std::cout<<"Cant kick client with fd: "<<kickedFileDescriptor<<std::endl;
            }
        }
    });

    while (true)
    {
        std::cout<<"choose test:"<<std::endl;
        getline(std::cin,command);
        ParsedMessage msg{command};
        msg.parseMessage();
        commandExecutor.executeCommand(msg);

    }
        




    return 0;
}