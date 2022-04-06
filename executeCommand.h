#pragma once
#include <string>
#include <map>
#include <functional>
#include "parsedMessage.h"

class CommandExec{
    public:
        CommandExec(){};
        ~CommandExec(){};
        void registerCommand(std::string command, std::function<void(ParsedMessage)> commandExecution );
        void executeCommand(ParsedMessage & message);
    private:
        std::map<std::string,std::function<void(ParsedMessage)> > commands;
};


