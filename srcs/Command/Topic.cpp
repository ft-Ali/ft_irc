
// /topic #channel                  -> return current topic
// /topic #channel : "new topic"    -> set new topic

#include "../../inc/Channel.hpp"
#include "../../inc/Server.hpp"
#include <cerrno>
#include <cstring>

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
    size_t startPos = cmd.size() + 1 + channelName.size(); // +1 for space between cmd and channelName
    std::string newTopic;
    if (startPos < message.size()) {
        newTopic = message.substr(startPos, message.size() - startPos);
    } else {
        newTopic = "";
    }
    std::cout << "newTopic: [" << newTopic << "] --size: " << newTopic.size() << std::endl;
    std::cout << message << "\n";

    try {
        size_t idx = 0;    //check if channelName exists
        while (idx < _channels.size() && _channels[idx]->getName() != channelName)
            idx++;
        if (idx >= _channels.size())
            throw std::invalid_argument("Channel not found");
        //if /topic and topic is empty
        if(newTopic.empty() && _channels[idx]->getTopic().empty())
            throw std::invalid_argument("No topic set");
        //if topic is empty and current client is operator then set NEW topic
        if (newTopic.empty() == false
            && ((_channels[idx]->getEditTopic() == false && _channels[idx]->checkOperatorList(_clients[clientFd-4]))
            || _channels[idx]->getEditTopic() == true)) {
                _channels[idx]->setTopic(newTopic);
        }
        //return CURRENT topic
        std::string response = ": server_name 332 " + _clients[clientFd-4]->getNickName() + " " + channelName + " " + _channels[idx]->getTopic() + "\r\n";
        std::cout << "-> channel " << channelName << "'s topic: [" << _channels[idx]->getTopic() << "]\n";
        int returnVal = send(_clients[clientFd-4]->getFd(), response.c_str(), response.size(), 0);
        if (returnVal == -1) {
            std::cerr << "send failed with error: " << strerror(errno) << std::endl;
            return;
        }
        std::cout << "returnVal: [" << returnVal << "]";
    }
    catch (std::invalid_argument &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}
