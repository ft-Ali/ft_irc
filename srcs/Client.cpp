#include "../inc/Client.hpp"

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
		this->_ip = src._ip;
		this->_isConnected = src._isConnected;
	}
	return *this;
}

void Client::removeJoinedChannel(Channel *channel){
	if(!channel){
		std::cout << "channel empty \n";
		return ;
	}
	std::vector<Channel*>::iterator it = std::find(_joinedChannels.begin(), _joinedChannels.end(), channel);
    if (it != _joinedChannels.end()) 
        _joinedChannels.erase(it);
  
}

void Client::print() {
    std::cout << std::endl
//     << "Client username: " << this->getUserName() << std::endl
//     << "Ipaddr: " << this->getIpaddr()<< std::endl
//     << "Port: " << this->getPort() << std::endl 
//     << "Nick: " << this->getNickName() << std::endl 
//     << "isConnected: " << this->isConnected() << std::endl
//     << "isOperator: " << this->isOperator() << std::endl
//     << "isRegistered: " << this->isRegistered() << std::endl << std::endl;
}