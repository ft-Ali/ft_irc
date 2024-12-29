
#include "../../inc/Channel.hpp"
#include "../../inc/Server.hpp"
#include <cerrno>
#include <cstring>

std::string getChannel(std::string message, std::string cmd) {
    
    std::string prompt_channelName;
    size_t hashPos = message.find('#', cmd.size());
    if (hashPos == std::string::npos) {
        return "";
    }
    prompt_channelName = message.substr(hashPos, message.find(' ', hashPos) - hashPos);

    return prompt_channelName;
}

std::string getTopic(std::string cmd, std::string channel, std::string message) {

    size_t startPos = cmd.size() + 1 + channel.size(); // +1 for space between cmd and prompt_channelName
    std::string newTopic;
    if (startPos < message.size()) {
        newTopic = message.substr(startPos, message.size() - startPos);
    } else {
        newTopic = "";
    }
    return newTopic;
}

void    Server::sendTopic(Client *client, Channel *channel) {

    std::string response = ":server_name 332 " + client->getNickName() + " " + channel->getName() + " " + channel->getTopic() + "\r\n";
    std::cout << "-> channel " << channel->getName() << "'s topic: [" << channel->getTopic() << "]\n";
    int returnVal = send(client->getFd(), response.c_str(), response.size(), 0);
    if (returnVal == -1) {
        std::cerr << "send failed with error: " << strerror(errno) << std::endl;
        return;
    }
}


// /topic
// /topic #channel                  -> return current topic
// /topic #channel : "new topic"    -> set new topic

void    Server::handleTopic(int clientFd, std::string message) {

    
    try {

    //1. /topic && /topic #channel

        std::string cmd = message.substr(0, message.find(' '));  //get cmd from message
        std::cout << "cmd: [" << cmd << "] --size: " << cmd.size() << std::endl;

        std::string prompt_channelName = getChannel(message, cmd); //get prompt_channelName from message
        std::cout << "prompt_channelName: [" << prompt_channelName << "] --size: " << prompt_channelName.size() << std::endl;

        std::string newTopic = getTopic(cmd, prompt_channelName, message); //get newTopic from message
        std::cout << "newTopic: [" << newTopic << "] --size: " << newTopic.size() << std::endl;

        Client *client = getClientByFds(clientFd);
        Channel *currentChannel = client->getCurrentChannel();

        if ((prompt_channelName.empty() == true && newTopic.empty() == true) || (prompt_channelName.empty() == false && newTopic.empty() == true) ) { //  /topic --> return current topic
        
            if (currentChannel->getTopic().empty()) {
                std::string response = ":server_name 331 " + client->getNickName() + " " + currentChannel->getName() + " :No topic is set\r\n";
                int returnVal = send(_clients[clientFd-4]->getFd(), response.c_str(), response.size(), 0);
                throw std::invalid_argument("No topic set for given channel");
                if (returnVal == -1) {
                    std::cerr << "send failed with error: " << strerror(errno) << std::endl;
                    return;
                }
            }
            else
                sendTopic(client, currentChannel);
        }

    //2. /topic #channel [topic] && /topic [topic]

        else if ((prompt_channelName.empty() == false && newTopic.empty() == false)
            || (prompt_channelName.empty() == true && newTopic.empty() == false)) {

            currentChannel->setTopic(newTopic);
            
            std::string response = ":" + client->getNickName() + "!" + client->getUserName() + "server_name" + " TOPIC " + currentChannel->getName() + " :" + newTopic + "\r\n";
            int returnVal = send(_clients[clientFd-4]->getFd(), response.c_str(), response.size(), 0);
            throw std::invalid_argument("No topic set for given channel");
            if (returnVal == -1) {
                std::cerr << "send failed with error: " << strerror(errno) << std::endl;
                return;
            }
        }
    }
    catch (std::invalid_argument &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

