// /topic
// /topic #channel                  -> return current topic
// /topic #channel : "new topic"    -> set new topic

#include "../../inc/Channel.hpp"
#include "../../inc/Server.hpp"
#include <cerrno>
#include <cstring>

std::string getChannel(std::string message, std::string cmd) {
    
    std::string channelName;
    size_t hashPos = message.find('#', cmd.size());
    if (hashPos == std::string::npos) {
        return "";
    }
    channelName = message.substr(hashPos, message.find(' ', hashPos) - hashPos);

    return channelName;
}

std::string getTopicName(std::string cmd, std::string channel, std::string message) {

    size_t startPos = cmd.size() + 1 + channel.size(); // +1 for space between cmd and channelName
    std::string newTopic;
    if (startPos < message.size()) {
        newTopic = message.substr(startPos, message.size() - startPos);
    } else {
        newTopic = "";
    }
    return newTopic;
}

void    Server::sendTopic(int clientFd, std::string channelName, std::string topic) {


    std::string response = ":server_name 332 " + _clients[clientFd-4]->getNickName() + " " + channelName + " " + topic + "\r\n";
    std::cout << "-> channel " << channelName << "'s topic: [" << topic << "]\n";
    int returnVal = send(_clients[clientFd-4]->getFd(), response.c_str(), response.size(), 0);
    if (returnVal == -1) {
        std::cerr << "send failed with error: " << strerror(errno) << std::endl;
        return;
    }
    std::cout << "returnVal: [" << returnVal << "]";
}

void    Server::handleTopic(int clientFd, std::string message) {

    
    try {

        std::string cmd = message.substr(0, message.find(' '));  //get cmd from message
    std::cout << "cmd: [" << cmd << "] --size: " << cmd.size() << std::endl;

        std::string channelName = getChannel(message, cmd); //get channelName from message
    std::cout << "channelName: [" << channelName << "] --size: " << channelName.size() << std::endl;
        
        Channel *channel = getChannelByName(channelName);
        if (channelName.empty()) {//  /topic --> return current topic
            if(channel->getTopic().empty())
                throw std::invalid_argument("No topic set");
           return sendTopic(clientFd, channelName, ""); 
        }
        
        std::string newTopic = getTopicName(cmd, channelName, message); //get newTopic from message
    std::cout << "newTopic: [" << newTopic << "] --size: " << newTopic.size() << std::endl;

        

            //if /topic and topic is empty
        if(newTopic.empty() && channel->getTopic().empty()) //if both new and set topi are empty
            throw std::invalid_argument("Missing input topic");
        if(channel->getTopic().empty())
            throw std::invalid_argument("No topic set");
        if (channel == NULL) { // error: channel not found
            throw std::invalid_argument("Channel not found");
        }
        //if topic is empty and current client is operator then set NEW topic
        if (newTopic.empty() == false
            && ((channel->getEditTopic() == false && channel->checkOperatorList(_clients[clientFd-4]))
            || channel->getEditTopic() == true)) {
                channel->setTopic(newTopic);
        }
        //return CURRENT topic
        sendTopic(clientFd, channelName, channel->getTopic());
    }
    catch (std::invalid_argument &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}
