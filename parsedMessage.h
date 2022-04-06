#pragma once
#include <iostream>
#include <vector>

class ParsedMessage{
    public:
        ParsedMessage(std::string message);
        bool parseMessage();
        std::vector<std::string>& returnParsedMessage();
    private:
        std::vector<std::string> parsedSplitedMessage;
        std::string message;
};