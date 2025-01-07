#include "../../inc/Channel.hpp"
#include "../../inc/Server.hpp"

 void Server::processJoin(std::string name, const std::string& message) {
    std::vector<std::string> cmd = splitArg(message, ' ');
    Client *client = getClientByName(name);
    if (!client) {
        return;
    }
    if (cmd.size() < 2 || cmd.size() > 3){
        sendClientResponse(client, ":[IRC] 461 " + client->getNickName() + " :Not enough parameters\r\n");
        return;
    }
    std::string channelName = cmd[1];
    std::string password = (cmd.size() > 2) ? cmd[2] : ""; 
    cmdJoin(channelName, password, client);
} 


void	Server::cmdJoin(std::string &Channelname, std::string &key, Client *client){
	
	std::vector<std::string> channels = splitArg(Channelname, ',');
	std::vector<std::string> keyLists = splitArg(key, ',');
	
	for(size_t i = 0; i < channels.size(); ++i){
		std::string channelName = channels[i];
		if (i < keyLists.size())
			key = keyLists[i];
		handleSingleJoin(channelName, key, client);
	}
}

void Server::handleSingleJoin(std::string &channelName, std::string &key, Client *client) {
    if (!client) 
        return;
    if (!client->hasValidHost()) {
        std::cerr << "Error: The client " << client->getNickName() << " does not have a valid host.\n";
        return;
    }

    if (channelName.empty() || channelName.size() == 1) {
        sendClientResponse(client, ":[IRC] 403 '" + channelName + "' JOIN :No such channel\r\n");
        return;
    }

    Channel* channel = NULL;
    if (!channelExist(channelName)) {
    if (key.empty())
        channel = new Channel(client, channelName);
    else
        channel = new Channel(client, channelName, key);

    if (!channel) {
        std::cerr << "Error: Failed to create the channel.\n";
        return;
    }
        if (!channel->parseChannelName(client)) {
            delete channel;
            return;
        }
        _channels.push_back(channel);
        client->setJoinedChannels(channel);
        client->setCurrentChannel(channel);
        clientToOperator(client, channel);
            std::cout << "channel size " << channel->size() << std::endl; 

    } else {
        channel = getChannelByName(channelName);
        if (!channel) {
            sendClientResponse(client, ":[IRC] 403 '" + channelName + "' :No such channel\r\n");
            return;
        }
        checkRestriction(channel, client, key);
            std::cout << "channel size joined" << channel->size() << std::endl; 

    }
}

void Server::clientToOperator(Client *client,Channel *channel){
  std::string namesResponse = ":[IRC] 353 " + client->getNickName() + " = " + channel->getName() + " :";
    const std::vector<Client*> members = channel->getMembers();
    for (size_t i = 0; i < members.size(); ++i) {
        if (!members[i]->hasValidHost()) {
            continue;
        }
        if (channel->checkOperatorList(members[i])) {
            namesResponse += "@";
        }
        namesResponse += members[i]->getNickName() + " ";
    }
    namesResponse += "\r\n";
    send(client->getFd(), namesResponse.c_str(), namesResponse.size(), 0);

   std::string host = client->getHost();
    if (host.empty()) {
        host = "unknown_host";
    }
    std::string welcomeMsg = ":" + client->getNickName() + "!" + client->getUserName() +  host + " JOIN " + channel->getName() + "\r\n";
    channel->broadcastInfoMessage(welcomeMsg);
}


void	Server::checkRestriction(Channel *channel, Client *client, std::string &key){
	
    if (channel->checkBanList(client)) {
        std::string response = ":[IRC] 474 " + client->getNickName() + " " + channel->getName() + " :Cannot join channel (+b)\r\n";
        send(client->getFd(), response.c_str(), response.size(), 0);
        return;
    }

    if (channel->getInvitOnly() && !channel->checkWhiteList(client)) {
        std::string response = ":[IRC] 441 " + client->getNickName()+ " " + channel->getName() + " :You are not invited(+i)\r\n";
        send(client->getFd(), response.c_str(), response.size(), 0);
        return;
    }

    if (channel->size() >= channel->getMaxMembers()) {
        std::string response = ":[IRC] 471 " + client->getNickName() + " " + channel->getName() + " :Cannot join channel (+l)\r\n";
        send(client->getFd(), response.c_str(), response.size(), 0);
        return;
    }
	if (!channel->getKey().empty()) {
        if (key != channel->getKey()) {
            std::string response = ":[IRC] 475 " + client->getNickName() + " " + channel->getName() + " :Invalid channel key\r\n";
            send(client->getFd(), response.c_str(), response.size(), 0);
            return;
        }
    }
	channel->addListMember(client);
    client->setJoinedChannels(channel);
    client->setCurrentChannel(channel);
    clientToOperator(client, channel);

}

Channel *Server::getChannelByName(std::string &name){
	
	for (size_t i = 0; i < _channels.size(); ++i) {
        if (_channels[i]->getName() == name)
            return _channels[i];
    }
	return NULL;
}

bool Server::channelExist(const std::string& name) {
    for (size_t i = 0; i < _channels.size(); ++i) {
        if (_channels[i]->getName() == name)
            return true;
    }
    return false;
}

std::vector<std::string> splitArg(const std::string &str, char delimiter) {
    std::vector<std::string> result;
    std::string name;

    for (size_t i = 0; i < str.size(); i++) {
        if (str[i] == delimiter) {
            if (!name.empty()) {
                name.erase(name.find_last_not_of(" \r\n") + 1);
                result.push_back(name);
                name.clear();
            }
        } else {
            name += str[i];
        }
    }
    if (!name.empty()) {
        name.erase(name.find_last_not_of(" \r\n") + 1);
        result.push_back(name);
    }
    return result;
}