#include "../inc/Client.hpp"
#include <strings.h>


Client::Client() {
    bzero(this, sizeof(Client));
    _fd = -1;
}

Client &Client::operator=(Client const &src){
	if (this != &src){
		this->_nickname = src._nickname;
		this->_username = src._username;
		this->_fd = src._fd;
        this->_port = src._port;
		// this->_channelsInvite = src._channelsInvite;
		this->_ip = src._ip;
		this->_isConnected = src._isConnected;
	}
	return *this;
}

void Client::removeJoinedChannel(std::vector<Channel*>& vec, Channel *channel){

	std::vector<Channel *>::iterator it = std::find(vec.begin(), vec.end(), channel);
	if(it != vec.end())
		vec.erase(it);
}