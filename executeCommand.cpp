#include "executeCommand.h"

void CommandExec::registerCommand(std::string command, std::function<void(ParsedMessage)> commandExecution ){
            commands.insert(std::pair<std::string, std::function<void(ParsedMessage)>>(command,commandExecution) );
}

void CommandExec::executeCommand(ParsedMessage & message){
            // message ma juz stworzony sparsowany i podzielony zestaw
            commands.find(message.returnParsedMessage().at(0))->second(message);
}