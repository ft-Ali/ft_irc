#include "../../inc/Channel.hpp"
#include "../../inc/Server.hpp"
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

std::string getTopic(std::string message) {

    size_t startPos = message.find(':');
    std::string tmp;
    std::string newTopic;
    if (startPos < message.size()) {
        tmp = message.substr(startPos, message.size() - startPos);
        newTopic = tmp.substr(1, tmp.size() - 1);
    } else {
        newTopic = "";
    }
    std::cout << newTopic << std::endl;
    return newTopic;
}

void    Server::handleTopic(int clientFd, std::string message) {

    
    try {

        std::string cmd = message.substr(0, message.find(' '));  //get cmd from message
        std::cout << "> cmd: [" << cmd << "] --size: " << cmd.size() << std::endl;

        std::string prompt_channelName = getChannel(message, cmd); //get prompt_channelName from message
        std::cout << "> prompt_channelName: [" << prompt_channelName << "] --size: " << prompt_channelName.size() << std::endl;

        std::string newTopic = getTopic(message); //get newTopic from message
        std::cout << "> newTopic: [" << newTopic << "] --size: " << newTopic.size() << std::endl;

        Client *client = getClientByFds(clientFd); //get pointer to client
        Channel *currentChannel = client->getCurrentChannel(); //get pointer to channel
        if(currentChannel == NULL)
            throw std::invalid_argument("No channel set for client");

    //1. /topic [#channel] [-] --> see topic

        if ((!prompt_channelName.empty() && newTopic.empty()) || (prompt_channelName.empty() && newTopic.empty())) {
            if (currentChannel->getTopic().empty()) {
                // std::string response = ":server_name 331 " + client->getNickName() + " " + currentChannel->getName() + " :No topic is set" + "\r\n";
                std::string response = "-!- No topic set for " + currentChannel->getName() + "\r\n";
                send(client->getFd(), response.c_str(), response.size(), 0);
                throw std::invalid_argument("No topic set for given channel");
            }
            else {
                // std::string response = ":server_name 332 " + client->getNickName() + " " + currentChannel->getName() + " " + currentChannel->getTopic() + "\r\n";
                std::string response = "-!- Topic for " + currentChannel->getName() + ": " + currentChannel->getTopic();
                send(client->getFd(), response.c_str(), response.size(), 0);
            }
        }

    //3. /topic #channel [-] [topic] && /topic [-] [topic] --> set topic

        else if ((currentChannel->getEditTopic() == false && currentChannel->checkOperatorList(client) == true) || currentChannel->getEditTopic() == true) {
         //2. /topic [#channel] [:] --> clear
    
            if (!prompt_channelName.empty() && newTopic == ":") {
                newTopic.clear();
            }
            currentChannel->setTopic(newTopic);
            
            // std::string response = ":" + client->getNickName() + "!" + client->getUserName() + "server_name" + " TOPIC " + currentChannel->getName() + " :" + newTopic + "\r\n";
            std::string response = "-!- " + client->getNickName() + "changed the topic of " + currentChannel->getName() +"\r\n";
            send(client->getFd(), response.c_str(), response.size(), 0);
            currentChannel->broadcastMessage(client, response);
        }
        else if (currentChannel->getEditTopic() == false) {

            // std::string response = ":server_name 482 " + client->getNickName() + " " + currentChannel->getName() + " :You're not a channel operator\r\n";
            std::string response = currentChannel->getName() + " :You're not a channel operator\r\n";
            send(client->getFd(), response.c_str(), response.size(), 0);
            throw std::invalid_argument("You're not a channel operator");
        }
    }
    catch (std::invalid_argument &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

// 331: SET -TOPIC empty and no topic set for current channel
// 332: SEE -TOPIC send to client
// setting channel successfully (not code)
// 482: ACCESS FORBIDDEN -TOPIC client not operator