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

void Server::handleTopic(int clientFd, std::string message) {
    try {
        std::vector<std::string> cmdSize = splitArg(message,' ');
        std::string cmd = message.substr(0, message.find(' '));
        std::string prompt_channelName = getChannel(message, cmd);
        std::string newTopic = getTopic(message);
        if(cmdSize.size() > 3)
            if(cmdSize[3].find(':') != 0)
                std::cout << "error";

        Client *client = getClientByFds(clientFd);
        Channel *currentChannel = client->getCurrentChannel();
        if (currentChannel == NULL)
            throw std::invalid_argument("No channel set for client");

        if ((!prompt_channelName.empty() && newTopic.empty()) || (prompt_channelName.empty() && newTopic.empty())) {
            if (currentChannel->getTopic().empty()) {
                std::string response = ":server_name 331 " + client->getNickName() + " " + currentChannel->getName() + " :No topic is set\r\n";
                send(client->getFd(), response.c_str(), response.size(), 0);
            } else {
                std::string response = ":server_name 332 " + client->getNickName() + " " + currentChannel->getName() + " :" + currentChannel->getTopic() + "\r\n";
                send(client->getFd(), response.c_str(), response.size(), 0);
            }
            return;
        }

        if ((currentChannel->getEditTopic() == false && currentChannel->checkOperatorList(client) == true) || currentChannel->getEditTopic() == true) {
            if (!prompt_channelName.empty() && newTopic == ":") {
                newTopic.clear();
            }
            currentChannel->setTopic(newTopic);
            
            std::string response = ":server_name 332 " + client->getNickName() + " " + currentChannel->getName() + " :" + newTopic + "\r\n";
            send(client->getFd(), response.c_str(), response.size(), 0);
            currentChannel->broadcastMessage(client, response);
        } else if (currentChannel->getEditTopic() == false) {
            std::string response = ":server_name 482 " + client->getNickName() + " " + currentChannel->getName() + " :You're not a channel operator\r\n";
            send(client->getFd(), response.c_str(), response.size(), 0);
            throw std::invalid_argument("You're not a channel operator");
        }
    } catch (std::invalid_argument &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}
