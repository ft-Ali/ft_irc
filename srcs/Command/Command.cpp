#include "../inc/Channel.hpp"
#include "../inc/Server.hpp"

void	Server::cmdJoin(const std::string &nameChannel, std::string &key, std::string &nameMembers){
	Client client;
	
		if(!channelExist(_channels, nameChannel))
		{
			Channel *channel;
			if(key.empty())
				channel = new Channel(&client,nameChannel);
			else if(!key.empty())
				channel = new Channel(&client,nameChannel, key);
			_channels.push_back(channel);
		}
		else{
			Channel *channel = getChannelByName(nameChannel);
			checkRestriction(*channel, client, key);
		}
}
Channel *Server::getChannelByName(const std::string &name){
	 for (size_t i = 0; i < _channels.size(); ++i) {
        if (_channels[i]->getName() == name)
            return _channels[i];
    }
	return NULL;
}

void	Server::checkRestriction(Channel channel, Client client, std::string &key){
	
	if(channel.getInvitOnly()){
		if(channel.checkWhiteList(&client))
			channel.addListMember(&client);
		else
			std::cout << "Client not in invit list\n";
		}
	
	if(!channel.getKey().empty()){
		if(key != channel.getKey())
			std::cout << "Invalid password\n";
	}

	if(channel.checkBanList(&client))
		std::cout << "Cannot join u are ban bozo\n";
	
	if(_channels.size() >= channel.getmaxMembers())
		std::cout << "Channel full\n";

	channel.addListMember(&client);
}

bool	Server::channelExist(std::vector<Channel*> &vec, const std::string &name){

		std::vector<Channel*>::iterator it = std::find(vec.begin(), vec.end(), name);
		if(it != vec.end())
			return true;
		return false;
}