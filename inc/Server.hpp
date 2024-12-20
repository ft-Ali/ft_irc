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

#include "Client.hpp"
#include "Channel.hpp"
#pragma once

class Server {
	private:
		int _port; //av[1]
		std::string _password;  //av[2]
		int _serSocketFd;
        int _suffix;

		static Server* instance;
		std::vector<Client*> _clients; //list Fd et IP address clients
		std::vector<Channel*> _channels; //-> vector of channels
		std::vector<struct pollfd> fds; //-> vector of pollfd
		std::map<int, bool> _authenticatedClients; //-> map of client fds and authentication status
		std::map<int, bool> _clientRegistered; //-> map of client fds and registration status
		std::map<int, std::string> _clientUsers; //-> map of client fds and users
		std::map<int, std::string> _clientNicks; //-> map of client fds and nicks
		
	public:
		Server(int port, std::string password) : _port(port), _password(password), _serSocketFd(-1), _suffix(0), fds(0) {
			if(instance != NULL) {
				throw(std::runtime_error("error: Server instance already exists"));
			}
			instance = this;
		};
		~Server() {
			closeServer();
		 
			instance = NULL;
			
		};
		void serverInit();
		void serverLoop();
		void handleNewConnection();
		void handleClientMessage(int i);
		static void signalHandler(int signal);
		void handleUser(int clientFd);
		void handleNick(int clientFd, const std::string& message);
		void handlePass(int clientFd, const std::string& message, size_t i);
		void processJoin(std::string Client, const std::string& message);
		void closeServer();
		void handleCap(int clientFd, const std::string& message);
		/************************************COMMAND********************************************/
		void cmdJoin(std::string &nameChannel, std::string &key, Client *client);
		bool channelExist(const std::string& name);
		void checkRestriction(Channel &channel, Client *client, std::string &key);
		void handleSingleJoin(std::string &channelName, std::string &key, Client *client);
		Channel *getChannelByName(std::string &name);
		void    cmdPart(const std::string &message, std::vector<std::string>&arg ,Client *client);
		void	cmdPartAll(Client* client);
		void    cmdPartMulti(const std::string &message, std::vector<std::string>&arg ,Client *client);
		Client *getClientByName(std::string &name);
		std::string getClientByFd(const int &ClientFd);
		void processPart(Client *client, std::string &command);
		void handlePrivMsg(const std::string& line, int clientFd);
		/***************************************************************************************/
		// void listen();
		// void accept();
		// void read();
		// void send();
		// void close();
};
std::vector<std::string> splitArg(const std::string &str, char delimiter);
