#include "../../inc/Server.hpp"
#include "../../inc/Channel.hpp"
#include <sys/un.h>
void Server::processPart(Client* client, std::string& command) {
    if (!client) {
        return;
    }
    std::string msg = "";
    size_t cmdIdx = command.find(' ', 4);
    if (cmdIdx != std::string::npos)
        command = command.substr(cmdIdx + 1);
    
    std::vector<std::string> arg = splitArg(command, ' ');
    if (arg.empty()) {
        std::cout << "Error: No channel specified in the PART command.\n";
        return;
    }
    size_t msgIdx = command.find(':');
    if (msgIdx != std::string::npos) {
        msg = command.substr(msgIdx + 1);
    }
    std::vector<std::string> channelNames = splitArg(arg[0], ',');
    cmdPart(msg, channelNames, client);
    
}

void Server::cmdPart(std::string& msg, std::vector<std::string>& channelNames, Client* client) {
    for (std::vector<std::string>::iterator it = channelNames.begin(); it != channelNames.end(); ++it) {
        Channel* channel = getChannelByName(*it);
        if (!channel) {
            continue;
        }
        if (!channel->checkListMembers(client)) {
            std::string response = ":[IRC] 442 " + client->getNickName() + " " + *it + " PART :You're not on that channel\r\n";
            send(client->getFd(), response.c_str(), response.size(), 0);
            continue;
        }
        std::string partMessage = ":" + client->getNickName() + "!" + client->getHost() + 
                                  " PART " + *it + " :" + msg + "\r\n";
        if (channel->checkOperatorList(client)) {
            channel->removeOperator(client);
        }
        channel->leaveChannel(client);
        client->removeJoinedChannel(channel);
        channel->setNewOperator();
        channel->broadcastMessage(client, partMessage);
        if (channel->size() == 0) {
            removeEmptyChannel(channel);
        }
    }
}

void Server::removeEmptyChannel(Channel* channel) {
    if (channel->size() == 0) {
        std::vector<Channel*>::iterator it = std::find(_channels.begin(), _channels.end(), channel);
        if (it != _channels.end()) {
                delete *it;
                _channels.erase(it);
        }
    }
}

Client *Server::getClientByName(std::string &name){
	for (size_t i = 0; i < _clients.size(); ++i) {
        if (_clients[i]->getNickName() == name)
            return _clients[i];
    }
	return NULL;
}
