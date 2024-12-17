#include "../../inc/Channel.hpp"
#include "../../inc/Server.hpp"

// exemple command plusieur channel JOIN #channel1,#channel2,#channel3 key1,,key3

std::vector<std::string> splitArg(const std::string &str, char delimiter){
	std::vector<std::string> result;
	std::string name;

	for (size_t i = 0; i < str.size(); i++)
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

void	Server::cmdJoin(std::string &Channelname, std::string &key, Client *client){
	
	std::vector<std::string> channels = splitArg(Channelname, ',');
	std::vector<std::string> keyLists = splitArg(key, ',');
	
	// if (channels.size() != keyLists.size()) {
    //     std::cout << "Error: Number of channels and keys do not match.\n";
    //     return;
    // }
	
	for(size_t i = 0; i < channels.size(); ++i){
		std::string channelName = channels[i];
		std::string keys = "";

		if (i < keyLists.size())
			key = keyLists[i];
		handleSingleJoin(channelName, key, client);
	}
	}

void Server::handleSingleJoin(std::string &channelName, std::string &key, Client *client){

	if(!channelExist(channelName)){
		Channel *channel;
		std::cout << "Channel '" << channelName << "' created and joined.\n";
		if(key.empty())
			channel = new Channel(client, channelName);
		else
			channel = new Channel(client, channelName, key);
		std::cout << &channel << std::endl;
		_channels.push_back(channel);
	}
	else{
		std::cout << "Channel " << channelName << " join.\n";
		Channel *channel = getChannelByName(channelName);
		std::cout << "copy " << &channel << std::endl;
		checkRestriction(*channel, client, key);
	}
}


void	Server::checkRestriction(Channel &channel, Client *client, std::string &key){
	
	if (channel.checkBanList(client)) {
        std::cout << "Cannot join " << channel.getName() << ", you are banned.\n";
        return;
    }
	if (channel.getInvitOnly() && !channel.checkWhiteList(client)) {
        std::cout << "Cannot join " << channel.getName() << ", invite-only mode.\n";
        return;
    }
	if (!channel.getKey().empty()) {
        if (key != channel.getKey()) {
            std::cout << "Invalid key for " << channel.getName() << ".\n";
            return;
        }
    }
    // if (_channels.size() >= channel.getmaxMembers()) {
    //     std::cout << "Cannot join " << channel.getName() << ", channel is full.\n";
    //     return;
    // }
	channel.addListMember(client);
	std::cout << "Joined " << channel.getName() << " successfully.\n";
}

Channel *Server::getChannelByName(std::string &name){
	
	for (size_t i = 0; i < _channels.size(); ++i) {
        if (_channels[i]->getName() == name)
            return _channels[i];
    }
	std::cout << "prout:\n";
	return NULL;
}

bool Server::channelExist(const std::string& name) {
    for (size_t i = 0; i < _channels.size(); ++i) {
        if (_channels[i]->getName() == name)
            return true;
    }
    return false;
}
