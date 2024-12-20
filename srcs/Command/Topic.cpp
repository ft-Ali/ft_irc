
// returns the #Channelname topic

///TOPIC #channelname :New topic text
// -> sets the topic to :...

#include "../../inc/Channel.hpp"
#include "../../inc/Server.hpp"

//1. recup channelName
//2. check operator
//3. set TOPIC


//TOPIC #channelname
std::string cutCmdInMessage(std::string message, std::string cmd) {
    
    size_t hashPos = message.find('#', cmd.size());
    std::string channelName = message.substr(hashPos, message.find(' ', hashPos) - hashPos);
    return channelName;
}

void    Server::handleTopic(int clientFd, std::string message) {

    (void)clientFd;

    std::string cmd = message.substr(0, message.find(' '));
    std::cout << cmd << std::endl;
    std::string channelName = cutCmdInMessage(message, cmd);
    std::cout << channelName << std::endl;
    std::string newTopic = message.substr(cmd.size()+channelName.size()+2);

    std::cout << "newTopic: [" << newTopic << "]\n";

    if (newTopic.empty()) {
        int idx = 0;

        while (_channels[idx]->getName() != channelName)
            idx++;
        if (_channels[idx]->checkOperatorList(_clients[clientFd - 4])) {
            std::cout << _channels[idx]->getTopic() << std::endl;
        }
    }

}
