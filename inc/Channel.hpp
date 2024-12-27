#include <vector>
#include <iostream>       
#include <typeinfo>      
#include <exception>
#include <ctime>
#include <iterator>
#include <algorithm>
#include <iostream>


#pragma once
#include "Client.hpp"

class Client;
class Channel{
	private:
		std::string _name;
		std::string _topic;
		std::string _key;
		std::time_t _creationTime;
		size_t _maxMembers;
		bool _editTopic;
		bool _invitOnly;
		bool _isOperator;
		std::vector<Client*> _members;
    	std::vector<Client*> _whiteList;
    	std::vector<Client*> _banList;
    	std::vector<Client*> _operatorList;
		std::vector<char> _modes;
	public:
		Channel(Client *client, std::string const &ChannelName, const std::string &key);
		Channel(Client *client, std::string const &ChannelName);
		~Channel();
		void parseChannelName();
		std::string getName();
		bool isOnList(const std::vector<Client*>& vec, Client *client);
	 	size_t size() const {
        	return _members.size(); // Retourne le nombre de membres
    	}
		bool getInvitOnly();//-i +i 
		void setInvitOnly();
		void undoInvitOnly();
		
		std::string getTopic();//-t +t
		bool getEditTopic() {return this->_editTopic;}
		void setTopic(std::string &topicName);

		std::string getKey();//-k +k
		void setKey(std::string &key);
		void undoKey();

		void setOperator( Client *client);//-o +o
		void removeOperator(Client* client);
		bool checkOperatorList(Client *client);
		bool isOperator(){return this->_isOperator;}
		bool checkListMembers(Client *client);
		void addMember(std::vector<Client*>& vec, Client *client);
		void addListMember(Client *client);

		void removeMember(Client *client);
		void removeClientList(std::vector<Client*>& vec, Client *client);
		size_t getMaxMembers();
		void setmaxMembers(size_t size);
		void leaveChannel(Client *client);
		bool checkWhiteList(Client *client);
		void addToWhiteList(Client *client);
		void removeToWhiteList(Client *client);

		bool checkBanList(Client *client);
		void addBanList(Client *client);
		void removeBanList(Client *client);
		
		std::string getModes() const;
    	void addMode(char mode);
	    void removeMode(char mode);
		std::vector<Client*> getMembers(){return this->_members;}
		size_t getSizeVec(std::vector<Client*> &vec){return vec.size();}
		void clearVec(std::vector<Client*> &vec, Channel *channel);
    	std::string getCreationTime() const;
		void broadcastMessage(Client* sender, const std::string& message);
		/*******************************MODE***********************************/
		void manageInvit(Client *client, Channel *channel, bool add);
		void manageTopic(Channel *channel, bool add);
		void manageKey(Channel *channel, std::string &key, bool add);
		void manageOperator(Channel *channel, Client *client, bool add);
		void manageSizeChannel(Channel *channel, size_t size,bool add);
		
};

