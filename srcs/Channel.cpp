#include "../inc/Channel.hpp"
#include "../inc/Server.hpp"

Channel::Channel(Client *client,const std::string  &ChannelName) : _name(ChannelName), _topic(""), _key("")
 ,_maxMembers(200), _editTopic(false), _invitOnly(false), _isOperator(false) {

    _members.push_back(client);
    _operatorList.push_back(client);
    _creationTime = time(NULL);
};

Channel::Channel(Client *client, const std::string  &ChannelName, const std::string &key) : _name(ChannelName), _topic(""), _key(key),
  _maxMembers(200),_editTopic(false), _invitOnly(false), _isOperator(false) {

    _members.push_back(client);
    _operatorList.push_back(client);
    _creationTime = time(NULL);
};

Channel::~Channel(){}

/**************************SETTERS*******************************/

void Channel::setInvitOnly(bool actived){
	if(actived)
		_invitOnly = true;
	else
		_invitOnly = false;
}

void Channel::setTopic(std::string &topicName){this->_topic = topicName;}

void Channel::setKey(std::string &key){this->_key = key;}

void Channel::setMaxMembers(size_t size){this->_maxMembers = size;}

void Channel::setTopicMode(bool actived){
	if(actived)
		_editTopic = true;
	else
		_editTopic = false;
}

void Channel::setOperator(Client *client){
	addMember(_operatorList, client);
}

void Channel::setNewOperator(){
	if(_members.size() >=1){
		std::vector<Client*>::iterator it = _members.begin();
		if(!checkOperatorList(*it))
			setOperator(*it);
	}
}

/**************************GETTERS*******************************/
std::string Channel::getTopic(){return this->_topic;}

bool Channel::getInvitOnly(){return this->_invitOnly;}

size_t Channel::getMaxMembers(){return this->_maxMembers;}

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

std::string Channel::getKey() {return this->_key;}

std::string Channel::getName() {return this->_name;}

/*******************************ADD******************************/
void Channel::addMember(std::vector<Client*> &vec, Client *client){
	vec.push_back(client);
}

void Channel::addMode(char mode, char sign) {
    if (sign != '+' && sign != '-')
        return;
    if (mode != 't' && mode != 'k' && mode != 'o' && mode != 'l' && mode != 'i')
        return;

    std::string fullMode = std::string(1, sign) + mode;
    for (std::vector<std::string>::iterator it = _modes.begin(); it != _modes.end(); ++it) {
        if ((*it)[1] == mode) { 
            _modes.erase(it);
            break;
        }
    }

    // Ajoute le mode avec le nouveau signe
    _modes.push_back(fullMode);
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
void Channel::clearVec(std::vector<Client*> &vec, Channel *channel){
	 for (size_t i = 0; i < channel->getSizeVec(vec); ++i) {
            if(vec[i])
				delete vec[i];
        }
	vec.clear();
}

void Channel::removeClientList(std::vector<Client*>& vec, Client *client){

	std::vector<Client*>::iterator it = std::find(vec.begin(), vec.end(), client);
    if (it != vec.end())
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

void Channel::removeMode(char mode, char sign){
	std::string fullMode;
	fullMode += sign;
	fullMode += mode;
	for(size_t i = 0; i < _modes.size(); ++i){
		if(_modes[i].find(fullMode)){
			_modes.erase(_modes.begin()+ i) ;
			return ;
		}
	}
}
void Channel::undoInvitOnly() {this->_invitOnly = false;}

void Channel::undoKey() {this->_key.clear();}

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
bool Channel::checkOperatorList(Client *client){

if(isOnList(_operatorList, client))
		return true;
	return false;
}

bool Channel::checkListMembers(Client *client){
	if(isOnList(_members, client))
		return true;
	return false;
}
/**************************UTILS*****************************/
bool Channel::parseChannelName(Client *client){
	size_t start = _name.find_first_not_of(' ');
    if (start == std::string::npos) {
        sendClientResponse(client, ":server_name 476 * :Channel name cannot be empty or only spaces\r\n");
        return false;
    }
	std::string trimName = _name.substr(start);
	if(trimName.size() > 50){
		sendClientResponse(client, ":server_name 476 " + trimName + "Channel name is too long\r\n");
	return false;
	}
	if(trimName[0] != '#'){
		sendClientResponse(client, ":server_name 476 " + trimName + "Channel name must start with '#'\r\n");
	return false;
	}
	if(trimName.find(' ') != std::string::npos){
		sendClientResponse(client, ":server_name 476 " + trimName + "Channel name contains invalid spaces\r\n");
	return false;
	}
	if(trimName.find("^G") != std::string::npos){
		sendClientResponse(client, ":server_name 476 " + trimName + "Channel name contains invalid characters\n");
	return false;
	}
	if(trimName.find(',') != std::string::npos){
		sendClientResponse(client, ":server_name 476 " + trimName + "Channel name contains commas\r\n");
	return false;
	}
	_name = trimName;
	return true;
}

void Channel::broadcastMessage(Client* sender, const std::string& msg) {
    for (std::vector<Client*>::iterator it = _members.begin(); it != _members.end(); ++it) {
        Client* member = *it;
        if (member != sender) {
            std::string response = ":" + sender->getNickName() + " PRIVMSG " + getName() + " " + msg + "\r\n";
            send(member->getFd(), response.c_str(), response.size(), 0);
        }
    }
}
