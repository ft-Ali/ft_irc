#pragma once
#include <Server.hpp>

class Client{
    private:
        int _fd;                                      // File descriptor for the client's socket
        int _port;                                    // Port number used by the client
        std::string _ip;                              // IP address of the client
        std::string _hostname;                        // Hostname of the client
        std::string _nickname;                        // Nickname chosen by the client
        std::string _username;                        // Username provided in the USER command
        bool _isConnected;                            // Client connection status
        bool _isOperator;

        std::vector<std::string> _channelsInvite;     // List of channels the client has joined
        std::chrono::time_point<std::chrono::system_clock> last_message_time; // Timestamp of the last message

	public:
        Client();
		Client(std::string &name, std::string &nick, int fd): _username(name), _nickname(nick), _fd(fd) {};
        ~Client() {};
        Client::Client(Client const &src){*this = src;}
	    Client &operator=(Client const &src);

        //---------------//Getters
	    int         getFd() { return _fd; }
	    // bool        invitedChannels(std::string &ChName);
	    bool 		isConnected() { return (_isConnected); }
	    std::string getNickName() { return _nickname; }
	    std::string getUserName() { return _username; }
	    std::string getIpaddr() { return _ip; }
	    std::string getHostname() { return _hostname; }
	    //---------------//Setters
	    void setFd(int fd) { _fd = fd; }
	    void setNickname(std::string& nickname){ _nickname = nickname; }
	    void isConnected(bool value) { _isConnected = value; }
	    void setUsername(std::string& username) { _username = username; }
	    void setIpAddr(std::string ipaddr) {_ip = ipaddr; }
	    //---------------//Methods
	    // void _channelsInvite(std::string &chname);
	    // void RmChannelsInvite(std::string &chname);
};
