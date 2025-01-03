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
    return newTopic;
}

void Server::handleTopic(int clientFd, std::string message) {
    try {
        std::vector<std::string> cmdSize = splitArg(message, ' ');
        std::string cmd = message.substr(0, message.find(' '));
        std::string prompt_channelName = getChannel(message, cmd);
        std::string newTopic = getTopic(message);
   
        Client *client = getClientByFds(clientFd);
        if (!client) {
            std::cerr << "Error: Client not found for fd " << clientFd << std::endl;
            return;
        }

        Channel *currentChannel = getChannelByName(prompt_channelName);
        if (!currentChannel) {
            sendClientResponse(client, ":[IRC] 403 " + prompt_channelName + " :No such channel\r\n");
            return;
        }

        // Case 1: No new topic provided
        if ((!prompt_channelName.empty() && newTopic.empty()) || (prompt_channelName.empty() && newTopic.empty())) {
            if (currentChannel->getTopic().empty()) {
                sendClientResponse(client, ":[IRC] 331 " + client->getNickName() + " " + currentChannel->getName() + " :No topic is set\r\n");
            } else {
                sendClientResponse(client, ":[IRC] 332 " + client->getNickName() + " " + currentChannel->getName() + " :" + currentChannel->getTopic() + "\r\n");
            }
            return;
        }
        if ((currentChannel->getEditTopic() == false && currentChannel->checkOperatorList(client) == true) || currentChannel->getEditTopic() == true) {
            if (!prompt_channelName.empty() && newTopic == ":") {
                newTopic.clear();
            }
            currentChannel->setTopic(newTopic);
            currentChannel->broadcastInfoMessage(":[IRC] 332 " + client->getNickName() + " " + currentChannel->getName() + " :" + newTopic + "\r\n");
        } 
        else if (currentChannel->getEditTopic() == false) {
            sendClientResponse(client, ":[IRC] 482 " + client->getNickName() + " " + currentChannel->getName() + " :You're not a channel operator\r\n");
        }
    } catch (std::invalid_argument &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        if (Client *client = getClientByFds(clientFd)) {
            sendClientResponse(client, ":[IRC] 400 :Unexpected error occurred\r\n");
        }
    }
}

