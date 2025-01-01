#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
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
#include <fstream>
#include <stdbool.h>
#include "../../inc/Channel.hpp"
#include "../../inc/Client.hpp"
#include "../../inc/Server.hpp"

#pragma once
class Channel;
class Client;
class Server;

class Bot {
	private:
		int _port;
		std::string _ip;
		std::string _pass;
		int _serSocketFdBot;
		std::string _nick;
		std::string _user;
	public:
		Bot() {};
		Bot(int port, std::string ip, std::string pass) : _port(port), _ip(ip), _pass(pass) {};
		~Bot() {}
		void botInit();
		void joinServer(int port, std::string ip, std::string pass);
		// void sendQuote();
		void setNick(std::string nick) { _nick = nick; };
		void setUser(std::string user) { _user = user; };

    // Ajout des getters pour port, ip et pass si nécessaire
		int getPort() const { return _port; }
		std::string getIp() const { return _ip; }
		std::string getPass() const { return _pass; }
};
