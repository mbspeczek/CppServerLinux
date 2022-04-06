#include <iostream>
#include "parsedMessage.h"

int main(void){

    ParsedMessage message("sendto 1 koza");
    message.parseMessage();
    
    for(auto& i: message.returnParsedMessage()){
        std::cout<<i<<std::endl;
    }

    return 0;
}