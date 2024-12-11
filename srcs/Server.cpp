#include "../inc/Server.hpp"

Server* Server::instance = NULL; // initialize instance to NULL, (instance = global that point to actual server [ON])

void Server::closeServer() {
	if (_serSocketFd != -1) {
        close(_serSocketFd);
        std::cout << "Server socket closed." << std::endl;
    }
    for (size_t i = 0; i < fds.size(); ++i) {
        close(fds[i].fd);
        std::cout << "Client FD " << fds[i].fd << " closed." << std::endl;
    }
	
	_authenticatedClients.clear();
	fds.clear();
	std::vector<struct pollfd>().swap(fds); // free memory of vector (clients)
}

bool Server::authenticateClient(int clientFd, const std::string& message, size_t i) {
	if (message == _password) {
		std::cout << "Client FD " << clientFd << " authenticated" << std::endl;
		_authenticatedClients[clientFd] = true;
		std::string response = "Welcome to the server!\n";
		send(clientFd, response.c_str(), response.size(), 0);
		return true; // client authenticated
	}
	else { // check if password is correct
			std::cout << "Client FD " << clientFd << " failed to authenticate" << std::endl;
			// std::cout << "password : '" << _password << "'"<<std::endl;
			// std::cout << "message : '" << message << "'" <<  std::endl;
			std::string response = "Authentication failed. Disconnecting.\n";
			send(clientFd, response.c_str(), response.size(), 0); // send response to client
			close(clientFd);
			_authenticatedClients.erase(clientFd); // remove client from authenticated clients
			fds.erase(fds.begin() + i); // remove client from fds
			return false;
		}
}

void Server::signalHandler(int signal) {
    if(signal == SIGQUIT) {
		std::cout << std::endl;
		std::cout << "SIGQUIT received. Closing server." << std::endl;
		instance->closeServer();
		exit(0);
	}
	else if(signal == SIGINT) {
		std::cout << std::endl;
		std::cout << "SIGINT received. Closing server." << std::endl;
		instance->closeServer();
		exit(0);
	}
}


void Server::handleClientMessage(int i) {
	char buffer[1024];
	int clientFd = fds[i].fd;

	int ret = recv(clientFd, buffer, sizeof(buffer) - 1, 0); // read message from client
	if (ret == -1) {
		throw(std::runtime_error("error: recv() failed"));
	}
	// std::cout << "ret : " << ret << std::endl;
	if (ret == 0) { // client disconnected
		std::cout << "Client FD " << clientFd << " disconnected" << std::endl;
		close(clientFd);
		_authenticatedClients.erase(clientFd);
		fds.erase(fds.begin() + i);
	} else { // message received from client
		buffer[ret -1] = '\0'; // null-terminate the message valgrind "\n" definitly lost

		std::string message(buffer); // convert buffer to string
		// std::cout << "Message from client (" << clientFd << "): '" << message <<"'"<< std::endl;
		if (!_authenticatedClients[clientFd]) { // client not authenticated
			if (!authenticateClient(clientFd, message, i)) { // authenticate client
				return ;
			}
		} else { // client authenticated -> handle message
			std::cout << "Message from client (" << clientFd << "): " << message << std::endl;
			std::string response = "Server received: " + message + "\n";
			send(clientFd, response.c_str(), response.size(), 0);
		}
	}
}


void Server::handleNewConnection() {
	int clientFd = accept(_serSocketFd, NULL, NULL); // accept new connection
	if (clientFd == -1) 
		throw(std::runtime_error("error: accept() failed"));
	struct pollfd client; // create new pollfd for new client
	client.fd = clientFd; // set fd to clientFd
	client.events = POLLIN; // set events to POLLIN
	client.revents = 0; // set revents to 0
	fds.push_back(client); // add new client to fds
	_authenticatedClients[clientFd] = false; // set client as unauthenticated
	std::cout << "New connected: FD " << clientFd << std::endl;
}

void Server::serverLoop() {

	while (true) {
		if (poll(fds.data(), fds.size(), -1) == -1) // -1 -> infinite timeout
			throw(std::runtime_error("error: poll() failed"));

		for (size_t i = 0; i < fds.size(); ++i) { // loop through all fds (clients)
			if (fds[i].revents & POLLIN) { // POLLIN -> there is data to read
				if (fds[i].fd == _serSocketFd) { // new connection
					handleNewConnection(); // accept new connection
				} else {
					handleClientMessage(i); // handle client message
				}
			}
		}
	}
}

void Server::serverInit() {

	struct sockaddr_in 	addr;
	struct pollfd		communication;

	addr.sin_family = AF_INET; //using IP V4
	addr.sin_port = htons(_port); //converts the port to network byte order
	addr.sin_addr.s_addr = INADDR_ANY; //listening to any IP

	_serSocketFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_serSocketFd == -1)
		throw(std::runtime_error("error: socket creation failed"));

	int en = 1;
	if (setsockopt(_serSocketFd, SOL_SOCKET, SO_REUSEADDR, &en, sizeof(en)) == -1)
		throw(std::runtime_error("error: failed to set option (O_NONBLOCK) on socket"));
	if (bind(_serSocketFd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
		throw(std::runtime_error("error: failed to bind _serSocketFd to _port"));
	if (listen(_serSocketFd, SOMAXCONN) == -1)
		throw(std::runtime_error("listen() faild"));

	communication.fd = _serSocketFd;
	communication.events = POLLIN;
	communication.revents = 0;
	fds.push_back(communication);

	std::cout << "Server started on port " << _port << std::endl;
}
