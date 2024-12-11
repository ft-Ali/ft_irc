#include "../inc/Channel.hpp"

		Channel::Channel(Client *client, std::string const &ChannelName, const std::string key) : _name(ChannelName), _topic(""), _key(key), _invitOnly(false), _editTopic(false), _sizeChannel(0){
			std::time_t time = std::time(nullptr);
			parseChannelName();
			_members.push_back(client);
			_operatorList.push_back(client);
			_creationTime = std::ctime(&time);
		};
		
		Channel::Channel(Client *client, std::string const &ChannelName) : _name(ChannelName), _topic(""), _key(""), _invitOnly(false), _editTopic(false), _sizeChannel(0){
			std::time_t time = std::time(nullptr);
			parseChannelName();
			_members.push_back(client);
			_operatorList.push_back(client);
			_creationTime = std::ctime(&time);
		};
bool Channel::validChar(char c){
	if(c == ' ' || c == '^G' || c == ',')
		return false;
	return true;
}

void Channel::parseChannelName(){
	bool isRegular = false;
	bool isLocal = false;

	if(_name.size() > 50)
		throw(std::invalid_argument("Inavlid size name"));
	if(_name[0]){
		if(_name[0] != '&' || _name[0] != '#')	
			throw(std::invalid_argument("Inavlid channel prefix"));
	}
	for(size_t i=1; i < _name.size();++i){
		if(!validChar(_name[i]))
			throw(std::invalid_argument("Invalid channel name"));
	}
}

bool Channel::getInvitOnly(){return this->_invitOnly;}

void Channel::setInvitOnly(){
	this->_invitOnly = true;
}

void Channel::undoInvitOnly(){
	this->_invitOnly = false;
}
std::string Channel::getTopic(){return this->_topic;}

void Channel::setTopic(const std::string &topicName){

}

std::string Channel::getKey(){return this->_key;}
void Channel::setKey(std::string &key){
	this->_key = key;
}
void Channel::undoKey(){
	this->_key = "";
}

void Channel::setOperator(Client *client){
	std::vector<Client *>::iterator i = 
}
bool Channel::checkOperatorList(Client *client){}
size_t Channel::getSizeChannel(){}
void Channel::setSizeChannel(){}
bool Channel::checkListMembers(Client *client){}
bool Channel::checkWhiteList(Client *client){}
bool Channel::checkBanList(Client *client){}


