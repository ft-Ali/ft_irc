#include "../inc/Server.hpp"
#include <map>

class Channel{
	private:
		std::string _name;
		std::string _topic;
		std::vector< Client> _members;
		//modes
	public:
		Channel(const std::string &channelName) : _name(channelName), _topic(""){};

};
