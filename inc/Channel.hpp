#include "../inc/Server.hpp"
#include <map>
#include <iostream>       
#include <typeinfo>      
#include <exception>
#include <ctime>
#include <iterator>

class Channel{
	private:
		std::string _name;
		std::string _topic;
		std::string _key;
		std::string _creationTime;
		size_t _sizeChannel;
		bool _editTopic;
		bool _invitOnly;
		std::vector<Client*> _members;
    	std::vector<Client*> _whiteList;
    	std::vector<Client*> _banList;
    	std::vector<Client*> _operatorList;
	public:
		Channel(Client *client, std::string const &ChannelName, const std::string key);
		Channel(Client *client, std::string const &ChannelName);
		
		void parseChannelName();
		bool validChar(char c);

		bool getInvitOnly();//-i +i 
		void setInvitOnly();
		void undoInvitOnly();
		
		std::string getTopic();//-t +t
		void setTopic(const std::string &topicName);

		std::string getKey();//-k +k
		void setKey(std::string &key);
		void undoKey();

		void setOperator(Client *client);//-o +o
		bool checkOperatorList(Client *client);
		
		size_t getSizeChannel();
		void setSizeChannel();
		bool checkListMembers(Client *Client);
		bool checkWhiteList(Client *client);
		bool checkBanList(Client *client);

		
};

