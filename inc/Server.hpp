/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plangloi <plangloi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/09 11:14:11 by alsiavos          #+#    #+#             */
/*   Updated: 2024/12/11 11:32:27 by plangloi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <vector>
#include <poll.h>


class Client {
	
};


// struct pollfd {
// 	int     fd; //-> file descriptor
// 	short   events;//-> requested events
// 	short   revents;//-> returned events
// };

class Server {
	private:
		int 		_port; //av[1]
		std::string password; //av[2]
		int 		_serSocketFd;
		static bool signal; //single ctrl-d -> send et double ctrl-d -> exit
		std::vector<Client> clients; //list Fd et IP address clients
		// std::vector<Channel> channels;
		std::vector<struct pollfd> fds; //-> vector of pollfd
	public:
		Server(int port) : _port(port) {};
		~Server() {};
		void serverInit();
		
		// void listen();
		// void accept();
		// void read();
		// void send();
		// void close();
};


#endif
