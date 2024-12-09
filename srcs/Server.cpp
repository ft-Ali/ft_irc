#include "../inc/Server.hpp"

Server::Server(int port) : port(port) {
}

Server::~Server() {
}

void Server::listen() {

}

void Server::init() {
	int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket == -1) {
		perror("socket");
		exit(1);
	}
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.s_addr = INADDR_ANY;

	bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
	if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
		perror("bind");
		exit(1);
	}
	listen(serverSocket, 5);

}


