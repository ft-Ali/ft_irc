#include "../../inc/Channel.hpp"
#include "../../inc/Server.hpp"

// exemple command plusieur channel JOIN #channel1,#channel2,#channel3 key1,,key3
 void Server::processJoin(std::string name, const std::string& message) {
    size_t spacePos = message.find(' ', 5);
    std::string channelName = message.substr(5, spacePos - 5);
    channelName.erase(channelName.find_last_not_of("\r\n") + 1);
    Client *client = getClientByName(name);
    if (!client) {
        std::cerr << "Error: Client '" << name << "' not found.\n";
        return;
    }
    
    std::string password;
    if (spacePos != std::string::npos) {
        password = message.substr(spacePos + 1);
        password.erase(password.find_last_not_of("\r\n") + 1);
    }
    cmdJoin(channelName, password, client);
} 


void	Server::cmdJoin(std::string &Channelname, std::string &key, Client *client){
	
	std::vector<std::string> channels = splitArg(Channelname, ',');
	std::vector<std::string> keyLists = splitArg(key, ',');
	
	// if (channels.size() != keyLists.size()) {
    //     std::cout << "Error: Number of channels and keys do not match.\n";
    //     return;
    // }
	
	for(size_t i = 0; i < channels.size(); ++i){
		std::string channelName = channels[i];
		if (i < keyLists.size())
			key = keyLists[i];
		handleSingleJoin(channelName, key, client);
	}
}

void Server::handleSingleJoin(std::string &channelName, std::string &key, Client *client) {
    if (!client) {
        std::cerr << "Error: Null client passed to handleSingleJoin.\n";
        return;
    }
    Channel* channel = NULL;
    if (!channelExist(channelName)) {
        // std::cout << "Channel '" << channelName << "' created and joined.\n";
        if (key.empty())
            channel = new Channel(client, channelName);
        else
            channel = new Channel(client, channelName, key);

        if (!channel) {
            std::cerr << "Error: Failed to create channel.\n";
            return;
        }

        _channels.push_back(channel);
        client->setJoinedChannels(channel);
    } else {
        // std::cout << "Channel " << channelName << " join.\n";
        channel = getChannelByName(channelName);
        if (!channel) {
            std::cerr << "Error: Failed to retrieve existing channel.\n";
            return;
        }
        checkRestriction(*channel, client, key);
    }
    // const std::vector<Channel*> chans = client->getJoinedChannels();
    // std::cout << "Client is now in " << chans.size() << " channel(s):\n";
    // for (size_t i = 0; i < chans.size(); ++i) {
    //     std::cout << " - " << chans[i] << " (" << chans[i]->getName() << ")\n";
    // }
}

void	Server::checkRestriction(Channel &channel, Client *client, std::string &key){
	
    if (channel.checkBanList(client)) {
        std::string response = ":server_name 474 " + client->getNickName() + " " + channel.getName() + " :Cannot join channel (+b)\r\n";
        send(client->getFd(), response.c_str(), response.size(), 0);
        return;
    }

    if (channel.getInvitOnly() && !channel.checkWhiteList(client)) {
        std::string response = ":server_name 441 " + client->getNickName()+ " " + channel.getName() + " :You are not invited\r\n";
        send(client->getFd(), response.c_str(), response.size(), 0);
        return;
    }

    if (channel.size() >= channel.getMaxMembers()) {
        std::string response = ":server_name 471 " + client->getNickName() + " " + channel.getName() + " :Cannot join channel (+l)\r\n";
        send(client->getFd(), response.c_str(), response.size(), 0);
        return;
    }
	if (!channel.getKey().empty()) {
        if (key != channel.getKey()) {
            std::string response = ":server_name 475 " + client->getNickName() + " " + channel.getName() + " :Invalid channel key\r\n";
            send(client->getFd(), response.c_str(), response.size(), 0);
            return;
        }
    }
    // if (channel.checkListMembers(client)) {
    //         std::string windowCommand = ":server_name NOTICE " + client->getNickName() + " :/window goto " + channel.getName() + "\r\n";
    //         send(client->getFd(), windowCommand.c_str(), windowCommand.size(), 0);
    //     return;
    // }
	channel.addListMember(client);
    client->setJoinedChannels(&channel);
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
std::vector<std::string> splitArg(const std::string &str, char delimiter){
	std::vector<std::string> result;
	std::string name;

	for(size_t i = 0; i < str.size(); i++)
	{
		if(str[i] == delimiter){
			if(!name.empty()){
				result.push_back(name);
				name.clear();
			}
		}
		else
			name+=str[i];
	}
	if(!name.empty())
		result.push_back(name);
    return result;
}