#include "parsedMessage.h"

ParsedMessage::ParsedMessage(std::string _message)
:message(_message)
{}

// command params 
bool ParsedMessage::parseMessage(){
    std::string s = message;
    std::string delimiter = " ";

    size_t pos = 0;
    std::string token;
    while ((pos = s.find(delimiter)) != std::string::npos) {
        token = s.substr(0, pos);
        parsedSplitedMessage.push_back(token);
        s.erase(0, pos + delimiter.length());
    }
    parsedSplitedMessage.push_back(s);
    
    if(parsedSplitedMessage.size()>0){
        return true;
    }
    return false;
}

std::vector<std::string>& ParsedMessage::returnParsedMessage(){
    return parsedSplitedMessage;
}