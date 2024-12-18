#pragma once

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sstream>
#include <stdlib.h>
#include <stdbool.h>
#include <vector>
#include <poll.h>
#include <map>
#include <stdlib.h>
#include <csignal>
#include <stdbool.h>

class Client {
    private:
        int _fd;                                      // File descriptor for the client's socket
        int _port;                                    // Port number used by the client
        std::string _ip;                              // IP address of the client
        std::string _hostname;                        // Hostname of the client
        std::string _username;                        // Username provided in the USER command
        std::string _nickname;                        // Nickname chosen by the client
        bool _isAuthentificated;
        bool _isRegistered;
        bool _isOperator;
        bool _isConnected;                            // Client connection status
        std::vector<Channel*> _joinedChannels;
	public:
        Client();
		// Client(int fd, std::string &nick, std::string &name):  _fd(fd), _nickname(nick), _username(name) {};
		Client(int fd, int clientPort, std::string clientIp, std::string user, std::string nick):  
            _fd(fd), _port(clientPort), _ip(clientIp), _username(user), _nickname(nick), _isAuthentificated(false), _isRegistered(false), _isOperator(false), _isConnected(false) {};
        ~Client() {};
        Client(Client const &src){*this = src;}
	    Client &operator=(Client const &src);

        //---------------//Getters
	    int         getFd() { return _fd; }
	    // bool        invitedChannels(std::string &ChName);
	    bool 		isConnected() { return (_isConnected); }
	    std::string getNickName() { return _nickname; }
	    std::string getUserName() { return _username; }
	    std::string getIpaddr() { return _ip; }
	    std::string getHostname() { return _hostname; }
        std::string getName() { return _username; }
	    //---------------//Setters
	    void setFd(int fd) { _fd = fd; }
	    void setNickname(std::string nickname){ _nickname = nickname; }
	    void setUsername(std::string username) { _username = username; }
	    void setIpAddr(std::string ipaddr) {_ip = ipaddr; }
        void setRegistered(bool value) { _isRegistered = value; }
	    void setConnected(bool value) { _isConnected = value; }
        void setOperator(bool value) { _isOperator = value; }
        void setAuthentificated(bool value) { _isAuthentificated = value; }
	    //---------------//Methods
	    // void _channelsInvite(std::string &chname);
	    // void RmChannelsInvite(std::string &chname);
};
