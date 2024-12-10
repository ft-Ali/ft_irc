#include "../inc/Server.hpp"

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
}