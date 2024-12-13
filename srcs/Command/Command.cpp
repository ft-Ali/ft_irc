#include "../inc/Channel.hpp"
#include "../inc/Server.hpp"

void	Server::cmdJoin(const std::string &nameChannel, std::string &key, std::string &nameMembers){
	
}

bool	Server::channelExist(std::vector<Channel*> &vec, std::string &name){

		std::vector<Channel*>::iterator it = std::find(vec.begin(), vec.end(), name);
		if(it != vec.end())
			return true;
		return false;
}