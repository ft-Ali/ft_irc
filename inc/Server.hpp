/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alsiavos <alsiavos@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/09 11:14:11 by alsiavos          #+#    #+#             */
/*   Updated: 2024/12/09 15:40:00 by alsiavos         ###   ########.fr       */
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

class Server {
	private:
		int port;
		int server_fd;
		int new_socket;
		struct sockaddr_in address;
		int addrlen;
		std::string message;
	public:
		Server(int port);
		~Server();
		void init();
		void listen();
		void accept();
		void read();
		void send();
		void close();
};


#endif
