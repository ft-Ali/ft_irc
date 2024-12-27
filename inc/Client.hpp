
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
#include "Channel.hpp"
#pragma once
class Channel;

class Client {
    private:
        int _fd;                                      // File descriptor for the client's socket
        int _port;                                    // Port number used by the client
        std::string _ip;                              // IP address of the client
        std::string _username;                        // Username provided in the USER command
        std::string _nickname;                        // Nickname chosen by the client
        bool _isAuthentificated;
        bool _isRegistered;
        bool _isOperator;
        bool _isConnected;                            // Client connection status
        std::vector<Channel*> _joinedChannels;
	public:
        Client();
		Client(int fd, int clientPort, std::string clientIp, std::string user, std::string nick):
            _fd(fd), _port(clientPort), _ip(clientIp), _username(user), _nickname(nick), _isAuthentificated(false), _isRegistered(false), _isOperator(false), _isConnected(false) {};
        ~Client() {}
        Client(Client const &src){*this = src;}
	    Client &operator=(Client const &src);

        //---------------//Getters
	    int         getFd() { return _fd; }
        int         getPort() { return _port; }
	    bool 		isConnected() { return _isConnected; }
        bool        isOperator() { return _isOperator; }
        bool        isRegistered() { return _isRegistered; }
	    std::string getNickName() { return _nickname; }
	    std::string getIpaddr() { return _ip; }
        std::string getUserName() { return _username; }
        std::vector<Channel*> getJoinedChannels() { return _joinedChannels; }

	    //---------------//Setters
	    void setFd(int fd) { _fd = fd; }
	    void setNickname(std::string nickname){ _nickname = nickname; }
	    void setUsername(std::string username) { _username = username; }
	    void setIpAddr(std::string ipaddr) {_ip = ipaddr; }
        void setRegistered(bool value) { _isRegistered = value; }
	    void setConnected(bool value) { _isConnected = value; }
        void setOperator(bool value) { _isOperator = value; }
        void setAuthentificated(bool value) { _isAuthentificated = value; }
        void setJoinedChannels(Channel *joinedChannel) { _joinedChannels.push_back(joinedChannel); }

	    //---------------//Methods
        void print();
        void removeJoinedChannel(Channel *channel);


};
