#include <iostream>
#include "executeCommand.h"

int main(void){
    CommandExec executor;
   

    executor.registerCommand("test",[&](ParsedMessage msg){
        std::cout<<msg.returnParsedMessage().at(0)<<std::endl;
    });
    
    executor.registerCommand("koza",[&](ParsedMessage msg){
        std::cout<<msg.returnParsedMessage().at(0)<<std::endl;
    });
    
    executor.registerCommand("brzoza",[&](ParsedMessage msg){
        std::cout<<msg.returnParsedMessage().at(0)<<std::endl;
    });
    
    std::vector<std::string> commands{"test 1 2","koza 1 2", "brzoza 1 2"};

    for(auto& i: commands){
        ParsedMessage msg{i};
        msg.parseMessage();
        executor.executeCommand(msg);
    }

  

    return 0;
}