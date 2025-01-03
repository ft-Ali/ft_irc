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
		std::vector<std::string> _modes;
	public:
		Channel(Client *client, std::string const &ChannelName, const std::string &key);
		Channel(Client *client, std::string const &ChannelName);
		~Channel();
		bool parseChannelName(Client *client);
		std::string getName();
		bool isOnList(const std::vector<Client*>& vec, Client *client);
	 	size_t size() const {
        	return _members.size(); // Retourne le nombre de membres
    	}
		bool getInvitOnly();//-i +i 
		void setInvitOnly(bool acived);
		void undoInvitOnly();
		
		std::string getTopic();//-t +t
		bool getEditTopic() {return this->_editTopic;}
		void setTopic(std::string &topicName);
		void setTopicMode(bool actived);
		size_t getSize(){return this->_maxMembers;};
		std::string getKey();//-k +k
		void setKey(std::string &key);
		void undoKey();
		void setNewOperator();
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
		void setMaxMembers(size_t size);
		void leaveChannel(Client *client);
		bool checkWhiteList(Client *client);
		void addToWhiteList(Client *client);
		void removeToWhiteList(Client *client);
		void broadcastInfoMessage(const std::string& message);
		bool checkBanList(Client *client);
		void addBanList(Client *client);
		void removeBanList(Client *client);
		
		std::string getModes();
    	void addMode(char mode, char sign);
	    void removeMode(char mode, char sign);
		std::vector<Client*> getMembers(){return this->_members;}
		size_t getSizeVec(std::vector<Client*> &vec){return vec.size();}
		size_t getSizeOpeList(){return _operatorList.size();}
		void clearVec(std::vector<Client*> &vec, Channel *channel);
    	std::string getCreationTime() const;
		void broadcastMessage(Client* sender, const std::string& message);
		/*******************************MODE***********************************/
	
		};

