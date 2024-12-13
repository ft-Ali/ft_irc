#include "../inc/Server.hpp"
#include <vector>
#include <iostream>       
#include <typeinfo>      
#include <exception>
#include <ctime>
#include <iterator>
#include <algorithm>
#include <iostream>


#pragma once
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
		std::string _modes;
		std::vector<Client*> _members;
    	std::vector<Client*> _whiteList;
    	std::vector<Client*> _banList;
    	std::vector<Client*> _operatorList;
	protected:
		std::vector<char> _modes;
	public:
		Channel(Client *client, std::string const &ChannelName, const std::string &key);
		Channel(Client *client, std::string const &ChannelName);
		
		void parseChannelName();
		bool isOnList(const std::vector<Client*>& vec, Client *client);

		bool getInvitOnly();//-i +i 
		void setInvitOnly();
		void undoInvitOnly();
		
		std::string getTopic();//-t +t
		void setTopic(Client *client, std::string &topicName);

		std::string getKey();//-k +k
		void setKey(std::string &key);
		void undoKey();

		void setOperator(const std::vector<Client*>& vec, Client *client);//-o +o
		void removeOperator(Client* client);
		bool checkOperatorList(Client *client);
		
		bool checkListMembers(Client *client);
		void addMember(const std::vector<Client*>& vec, Client *client);
		void removeMember(Client *client);
		void removeClientList(std::vector<Client*>& vec, Client *client);
		size_t getmaxMembers();
		void setmaxMembers(size_t size);
		
		bool checkWhiteList(Client *client);
		void addToWhiteList(Client *client);
		void removeToWhiteList(Client *client);

		bool checkBanList(Client *client);
		void addBanList(Client *client);
		void removeBanList(Client *client);
		
		std::string getModes() const;
    	void addMode(char mode);
	    void removeMode(char mode);

    	std::string getCreationTime() const;
};

