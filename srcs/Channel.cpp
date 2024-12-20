#include "../inc/Channel.hpp"

Channel::Channel(Client *client,const std::string  &ChannelName) : _name(ChannelName), _topic(""), _key(""),
_maxMembers(0) , _editTopic(false), _invitOnly(false) {

    parseChannelName();
    _members.push_back(client);
    _operatorList.push_back(client);
    _creationTime = time(NULL);
};

Channel::Channel(Client *client, const std::string  &ChannelName, const std::string &key) : _name(ChannelName), _topic(""), _key(key),
_maxMembers(0) , _editTopic(false), _invitOnly(false) {

    parseChannelName();
    _members.push_back(client);
    _operatorList.push_back(client);
    _creationTime = time(NULL);
};


/**************************SETTERS*******************************/

void Channel::setInvitOnly(){this->_invitOnly = true;}

void Channel::setTopic(Client *client, std::string &topicName){
	if(_editTopic == true && checkOperatorList(client))
		this->_topic = topicName;
	if(_editTopic == false)
		this->_topic = topicName;
}

void Channel::setKey(std::string &key){this->_key = key;}

void Channel::setmaxMembers(size_t size){this->_maxMembers = size;}

// void Channel::setOperator(const std::vector<Client*>& vec, Client *client){}


/**************************GETTERS*******************************/
std::string Channel::getTopic(){return this->_topic;}

bool Channel::getInvitOnly(){return this->_invitOnly;}

size_t Channel::getmaxMembers(){return this->_maxMembers;}

std::string Channel::getModes() const{
	std::string modes;
	for(size_t i = 0;i<_modes.size(); ++i){
		modes += _modes[i];
	}
	return modes;
}

std::string Channel::getCreationTime() const{
	std::string time = ctime(&_creationTime);
	return time;
}

std::string Channel::getKey(){return this->_key;}

std::string Channel::getName(){return this->_name;}

/*******************************ADD******************************/
void Channel::addMember(std::vector<Client*> &vec, Client *client){

	std::vector<Client *>::const_iterator it = std::find(vec.begin(), vec.end(), client);
	(void)it;
	// if(it != vec.end()){
	// 	std::cout << "Client already set \n";
	// 	std::cout << client->getName() << std::endl;
	// }
	// else
	// for(std::vector<Client*>::iterator it = _members.begin(); it != _members.end(); ++it){
		
	// 	std::cout <<"client :" << client->getName() << std::endl;

	// }
		vec.push_back(client);
}

void Channel::addMode(char mode){
	for(size_t i = 0; i < _modes.size(); ++i){
		if(mode == _modes[i])
			return ;
	}
	if(mode == 'i' ||mode == 't' ||mode == 'k' ||mode == 'o' ||mode == 'l')
		_modes.push_back(mode);
	else
		std::cout << "Invalid mod \n";
}

void Channel::addToWhiteList( Client *client){
	addMember(_whiteList, client);
}

void Channel::addBanList( Client *client){
	addMember(_banList, client);
}
void Channel::addListMember(Client *client){
	addMember(_members, client);
}

/*******************************REMOVE***************************/
void Channel::removeClientList(std::vector<Client*>& vec, Client *client){

	std::vector<Client *>::iterator it = std::find(vec.begin(), vec.end(), client);
	if(it != vec.end())
		vec.erase(it);
}

void Channel::removeMember( Client *client){
	removeClientList(_members, client);
	removeClientList(_banList, client);
	removeClientList(_operatorList, client);
	removeClientList(_whiteList, client);

}
void Channel::leaveChannel(Client *client){
	removeClientList(_members, client);
}
void Channel::removeOperator(Client* client){
	removeClientList(_operatorList, client);
}

void Channel::removeToWhiteList(Client *client){
	removeClientList(_whiteList, client);
}

void Channel::removeBanList(Client *client){
	removeClientList(_banList, client);
}

void Channel::removeMode(char mode){
	for(size_t i = 0; i < _modes.size(); ++i){
		if(mode == _modes[i]){
			_modes.erase(_modes.begin()+ i) ;
			return ;
		}
	}
}

void Channel::undoInvitOnly(){this->_invitOnly = false;}
void Channel::undoKey(){this->_key = "";}
/*******************************CHECK****************************/
bool Channel::isOnList(const std::vector<Client*>& vec, Client *client){
	
	std::vector<Client *>::const_iterator it = std::find(vec.begin(), vec.end(), client);
	if(it != vec.end())
		return true;
	return false;

}
bool Channel::checkBanList( Client *client){
	if(isOnList(_banList, client))
		return true;
	return false;
}
bool Channel::checkWhiteList( Client *client){
	
	if(isOnList(_whiteList, client))
		return true;
	return false;}
bool Channel::checkOperatorList( Client *client){

if(isOnList(_operatorList, client))
		return true;
	return false;
}

bool Channel::checkListMembers( Client *client){
	if(isOnList(_members, client))
		return true;
	return false;
}
/****************************************************************/
void Channel::parseChannelName(){

	if(_name.size() > 50)
		throw(std::invalid_argument("Inavlid size name"));
	if(_name[0] != '#')	
		throw(std::invalid_argument("Inavlid channel prefix"));
	if(_name.find(' ') != std::string::npos)
		throw(std::invalid_argument("Inavlid channel name"));
	if(_name.find("^G") != std::string::npos)
		throw(std::invalid_argument("Inavlid channel name"));
	if(_name.find(',') != std::string::npos)
		throw(std::invalid_argument("Inavlid channel name"));

}

