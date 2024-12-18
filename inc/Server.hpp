<<<<<<< HEAD
=======
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alsiavos <alsiavos@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/09 11:14:11 by alsiavos          #+#    #+#             */
/*   Updated: 2024/12/17 16:13:36 by alsiavos         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
>>>>>>> origin/meshmesh

#ifndef SERVER_HPP
# define SERVER_HPP

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


// struct pollfd {
// 	int     fd; //-> file descriptor
// 	short   events;//-> requested events
// 	short   revents;//-> returned events
// };

class Server {
	private:
		int 		_port;      //av[1]
		std::string _password;  //av[2]
		int 		_serSocketFd;
        int _suffix;

		static Server* instance;
<<<<<<< HEAD
		std::vector<Client> _clients; //list Fd et IP address clients
		std::vector<Channel> _channels;
=======
		// std::vector<Client> clients; //list Fd et IP address clients
		std::vector<Channel*> _channels;
>>>>>>> origin/meshmesh
		std::vector<struct pollfd> fds; //-> vector of pollfd
		std::map<int, bool> _authenticatedClients; //-> map of client fds and authentication status
		std::map<int, std::string> _clientNicks; //-> map of client fds and nicks
		std::map<int, bool> _clientRegistered; //-> map of client fds and registration status
		std::map<int, std::string> _clientUsers; //-> map of client fds and users
		// std::vector<Channel *> _channels; //-> vector of channels
		
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
		bool authenticateClient(int clientFd, const std::string& message, size_t i);
		static void signalHandler(int signal);
		void handleUser(int clientFd);
		void handleNick(int clientFd, const std::string& message);
		void handlePass(int clientFd, const std::string& message, size_t i);
		void processJoin(std::string Client, const std::string& message);
		void closeServer();
		void cmdJoin(std::string &nameChannel, std::string &key, Client *client);
		bool channelExist(const std::string& name);
		void checkRestriction(Channel &channel, Client *client, std::string &key);
		void handleSingleJoin(std::string &channelName, std::string &key, Client *client);
		Channel *getChannelByName(std::string &name);
		
		// void listen();
		// void accept();
		// void read();
		// void send();
		// void close();
};


#endif
