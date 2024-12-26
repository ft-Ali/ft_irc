
// /topic #channel                  -> return current topic
// /topic #channel : "new topic"    -> set new topic


#include "../../inc/Channel.hpp"
#include "../../inc/Server.hpp"

std::string cutCmdInMessage(std::string message, std::string cmd) {
    
    size_t hashPos = message.find('#', cmd.size());
    std::string channelName = message.substr(hashPos, message.find(' ', hashPos) - hashPos);
    return channelName;
}

void    Server::handleTopic(int clientFd, std::string message) {

    (void)clientFd;

    //get cmd from message
    std::string cmd = message.substr(0, message.find(' '));
    std::cout << "cmd: [" << cmd << "] --size: " << cmd.size() << std::endl;

    //get channelName from message
    std::string channelName = cutCmdInMessage(message, cmd);
    std::cout << "channelName: [" << channelName << "] --size: " << channelName.size() << std::endl;
    
    //get newTopic from message
    std::string newTopic = message.substr(cmd.size()+1+channelName.size(), message.size()); // +1 for space between cmd and channelName
    std::cout << "newTopic: [" << newTopic << "] --size: " << newTopic.size() << std::endl;

    //check if channelName exists
    int idx = 0;
    std::cout << message << "\n";
    if (newTopic.empty()) {
        std::cout << "-> channel " << channelName << "'s CURRENT topic: [" << _channels[idx]->getTopic() << "]\n";
    }
    while (_channels[idx] && _channels[idx]->getName() != channelName)
        std::cout << idx++ << std::endl;
    
    //if topic is empty and current client is operator then set NEW topic
    if (_channels[idx]->getTopic().empty()) {
        _channels[idx]->setTopic(_clients[clientFd - 4], newTopic);
        std::cout << "-> channel " << channelName << "'s NEW topic: [" << _channels[idx]->getTopic() << "]\r\n";
    } else { //else if topic is NOT empty return current topic
        std::string sentence = "-> channel "+ channelName + "'s CURRENT topic: [" + _channels[idx]->getTopic() + "]\r\n";
        const void *buff = sentence.c_str();

        std::cout << "-> channel " << channelName << "'s CURRENT topic: [" << _channels[idx]->getTopic() << "]\n";
        int returnVal = send(clientFd, buff, _channels[idx]->getTopic().size(), 0);
        std::cout << "returnVal: [" << returnVal << "]";
    }
}

