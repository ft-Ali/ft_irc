#include "../inc/Server.hpp"


//  void Server::processJoin(std::string name, const std::string& message) {
        // std::vector<Client*>::iterator it = 
//     size_t spacePos = message.find(' ', 5);
//     std::string channelName = message.substr(5, spacePos - 5);
//     channelName.erase(channelName.find_last_not_of("\r\n") + 1);

//     std::string password;
//     if (spacePos != std::string::npos) {
//         password = message.substr(spacePos + 1);
//         password.erase(password.find_last_not_of("\r\n") + 1);
//     }
//     cmdJoin(channelName, password, &client);
// } 


// Function that handles the PASS command sent by the client.
// This command is used to authenticate the client with a password.
// - If the password is correct, the client is authenticated and can proceed with sending the NICK and USER commands.
// - If the password is incorrect, the connection is closed for this client.

// Parameters:
// - clientFd: The file descriptor of the client sending the PASS command.
// - message: The received message containing the password (the "PASS " command part is removed).
// - i: The index of the client in the fds list (used to remove the client from fds when disconnecting).

void Server::handlePass(int clientFd, const std::string& message, size_t i) {
    // Extract the password from the message (remove the "PASS " part)
    std::string password = message.substr(5);
    password.erase(password.find_last_not_of("\r\n") + 1); // Clean up the newline characters

    // Check if the password is correct
    if (password == _password) {
        _authenticatedClients[clientFd] = true;
        _clientRegistered[clientFd] = false;  // Client is not fully registered yet
        _clients[clientFd - 4].setAuthentificated(true);
        _clients[clientFd - 4].setRegistered(false);

        std::cout << "Client FD " << clientFd << " authenticated" << std::endl;

        // Retrieve the nickname (if initialized before)
        std::string nickname = _clientNicks[clientFd];

        std::string response = "PASS OK. Using nickname: " + nickname + ".\n";
        send(clientFd, response.c_str(), response.size(), 0);

        // Automatically send NICK and USER after PASS
        handleNick(clientFd, "NICK " + nickname + "\r\n");
        handleUser(clientFd);
    } else {
        _authenticatedClients[clientFd] = false;
        _clients[clientFd].setAuthentificated(false);
        // Failed authentication: Close the connection
        std::string response = "Authentication failed. Disconnecting.\n";
        send(clientFd, response.c_str(), response.size(), 0);
        close(clientFd);
        _authenticatedClients.erase(clientFd);
        fds.erase(fds.begin() + i);
        std::cout << "Client FD " << clientFd << " disconnected due to incorrect password." << std::endl;
    }
}




// Function that handles the NICK command sent by the client.
// This command allows the client to set their nickname.
// - If the nickname is valid, it is stored for the client and the server sends a confirmation.
// - If a nickname is already set, the server sends a response asking for the USER command.

// Parameters:
// - clientFd: The file descriptor of the client sending the NICK command.
// - message: The received message containing the nickname (the "NICK " command part is removed).

void Server::handleNick(int clientFd, const std::string& message) {
    std::string newNickname;

    if (message.length() > 5) {
        newNickname = message.substr(5);
        newNickname.erase(newNickname.find_last_not_of("\r\n") + 1);
    } else {
        std::string response = "ERROR: No nickname provided.\n";
        send(clientFd, response.c_str(), response.size(), 0);
        return;
    }

    // Check if the nickname is already in use
    std::string baseNickname = newNickname;
    _suffix = 0;
    for (std::map<int, std::string>::iterator it = _clientNicks.begin(); it != _clientNicks.end(); ++it) {
        if (it->second == newNickname && _suffix > 0) {
            std::ostringstream oss;
            oss << baseNickname <<"("<< _suffix << ")";
            newNickname = oss.str();
        }
        _suffix++;
    }
    std::string oldNickname = _clientNicks[clientFd];
    _clientNicks[clientFd] = newNickname;

    std::string response = ":" + oldNickname + " NICK " + newNickname + "\n";
    send(clientFd, response.c_str(), response.size(), 0);

    std::cout << "Client FD " << clientFd << " changed nickname from " 
              << oldNickname << " to " << newNickname << std::endl;
}




// Function that handles the USER command sent by the client.
// This command is used to set the client's username and complete the registration process.
// - If the NICK command was not provided first, the server sends an error message asking for the NICK.
// - Once both NICK and USER commands are received, the client is considered fully registered.

// Parameters:
// - clientFd: The file descriptor of the client sending the USER command.

void Server::handleUser(int clientFd) {
    if (_clientNicks.find(clientFd) == _clientNicks.end() || _clientNicks[clientFd].empty()) {
        std::string response = "Please provide NICK first.\n";
        send(clientFd, response.c_str(), response.size(), 0);
        return;
    }

    // Utilise le USER du système si disponible, sinon le NICK existant
    const char* systemUser = getenv("USER");
    std::string username = systemUser ? systemUser : _clientNicks[clientFd];
    _clientUsers[clientFd] = username;
    _clients[clientFd].setUsername(username);
    _clientRegistered[clientFd] = true;
    _clients[clientFd].setRegistered(true);

    std::string welcome = ":localhost 001 " + _clientNicks[clientFd] + 
                          " :Welcome to the IRC Network, username: " + username + "\n";
    send(clientFd, welcome.c_str(), welcome.size(), 0);

    std::cout << "Client FD " << clientFd << " (nickname: " << _clientNicks[clientFd] 
              << ", username: " << username << ") fully registered." << std::endl;
}


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
		// std::cout << "Client FD " << clientFd << " authenticated" << std::endl;
		_authenticatedClients[clientFd] = true;
        _clients[clientFd].setAuthentificated(true);
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


// Function that processes incoming messages from clients.
// - The function checks if the client is authenticated, then processes different IRC commands like PASS, NICK, USER.
// - If the client is not fully registered, it asks for NICK and USER commands to complete the registration.
// - It also handles PING and QUIT commands to maintain the connection or close it.

// Parameters:
// - i: Index of the client in the `fds` list (used to access the file descriptor).

void Server::handleClientMessage(int i) {
    char buffer[1024];
    int clientFd = fds[i].fd;

    // Receive the message from the client
    int ret = recv(clientFd, buffer, sizeof(buffer) - 1, 0);
    if (ret <= 0) {
        // If the client disconnects, clean up and remove the client from the list
        std::cout << "Client FD " << clientFd << " disconnected" << std::endl;
        close(clientFd);
        _authenticatedClients.erase(clientFd);
        _clientNicks.erase(clientFd);
        _clientRegistered.erase(clientFd);
        fds.erase(fds.begin() + i);
        return;
    }

    buffer[ret] = '\0'; // Null-terminate the received message
    std::string message(buffer);

    // Step 1: Handle PASS command if the client is not authenticated
       if (!_authenticatedClients[clientFd]) {
        if (message.rfind("PASS ", 0) == 0) {
            handlePass(clientFd, message, i);

            // Auto-send NICK and USER after PASS
            if (_authenticatedClients[clientFd]) {
                std::ostringstream oss;
                oss << "guest" << clientFd;
                std::string nickname = oss.str();

                const char* systemUser = getenv("USER");
                std::string username = systemUser ? systemUser : "default_user"; // Default USER
                _clientNicks[clientFd] = nickname;                
                _clients[clientFd]->setNickname(nickname);

                _clientUsers[clientFd] = username;
                _clients[clientFd]->setUsername(username);

                // Send NICK and USER commands automatically
                // handleNick(clientFd, "NICK " + nickname + "\r\n");
                // handleUser(clientFd);
            }
        } else {
            std::string response = "Please provide a password using PASS <password>\n";
            send(clientFd, response.c_str(), response.size(), 0);
        }
        return;
    }

    // Step 2: Handle NICK command
    if (message.rfind("NICK ", 0) == 0) {
        handleNick(clientFd, message);
        return;
    }
    if (!_clientRegistered[clientFd]) {
        if (message.rfind("USER ", 0) != 0) {
            const char* systemUser = getenv("USER");
            if (!systemUser) {
                systemUser = getenv("LOGNAME");
            }
            std::string username = systemUser ? systemUser : "default_user";
            _clientUsers[clientFd] = username;
            _clients[clientFd]->setUsername(username);


            handleUser(clientFd);
            return;
        }
    }
    if(message.rfind("JOIN ", 0) == 0){
        std::string clientName;
        std::map<int , std::string>::iterator it = _clientNicks.find(clientFd);
        if(it != _clientNicks.end())
            clientName = it->second;
        // processJoin(clientName, message);
        return;
    }
     if (message.rfind("PING ", 0) == 0) {
        std::string pong = "PONG " + message.substr(5) + "\n";
        send(clientFd, pong.c_str(), pong.size(), 0);
        // std::cout << "PONG sent to client FD " << clientFd << std::endl;
        return;
    }
      if (message.rfind("MODE ", 0) == 0) {
        // std::cout << "Client " << clientFd << " sent MODE, ignoring for now." << std::endl;
        return;
    }
    if (message.rfind("QUIT", 0) == 0) {
        std::string response = "QUIT\n";
        send(clientFd, response.c_str(), response.size(), 0);
        close(clientFd);
        _authenticatedClients.erase(clientFd);
        _clientNicks.erase(clientFd);
        _clientRegistered.erase(clientFd);
        fds.erase(fds.begin() + i);
        std::cout << "Client FD " << clientFd << " disconnected" << std::endl;
        return;
    }

    if(message.rfind("PART ",0) == 0){
        std::string clientName;
        std::map<int , std::string>::iterator it = _clientNicks.find(clientFd);
        if(it != _clientNicks.end())
            clientName = it->second;
        Client *client = getClientByName(clientName);


    }
    // Handle other messages for registered clients
    std::cout << "Message from client (" << "plouf" << "): " << message << std::endl;
    std::string response = "Server received: " + message + "\n";
    send(clientFd, response.c_str(), response.size(), 0);
}



void Server::handleNewConnection() {

    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

    int clientFd = accept(_serSocketFd, (struct sockaddr*)&clientAddr,  &clientAddrLen); // Accept new connection
    if (clientFd == -1) 
        throw(std::runtime_error("error: accept() failed"));

    // char clientIp[INET_ADDRSTRLEN]; // Creates a char array to store the client’s IP address in human-readable form.
    // inet_ntop(AF_INET, &clientAddr.sin_addr, clientIp, INET_ADDRSTRLEN); // Converts the binary representation of the client’s IP address into a human-readable string 
    // int clientPort = ntohs(clientAddr.sin_port); // Converts the client’s port number from network byte order to host byte order and stores it in clientPort.

    struct pollfd client;
    client.fd = clientFd;
    client.events = POLLIN;
    client.revents = 0;
    fds.push_back(client);

    _clientNicks[clientFd] = "";
    _clientUsers[clientFd] = "";
    _authenticatedClients[clientFd] = false;
    _clientRegistered[clientFd] = false;
    // _clients[clientFd - 4].setNickname("");
    // _clients[clientFd - 4].setUsername("");
    // _clients[clientFd - 4].setAuthentificated(false);
    // _clients[clientFd - 4].setRegistered(false);


    // Get the system user name to use as a default nickname
    const char* systemUser = getenv("USER");
    if (!systemUser) {
        systemUser = getenv("LOGNAME"); // Fallback si USER n'est pas défini
    }

    // use the system USER if available, otherwise use a default nickname
    std::string nickname = systemUser ? systemUser : "anonymous_user";

    // Assign the nickname to the client
    _clientNicks[clientFd] = nickname;
    // _clients[clientFd - 4].setNickname(nickname);

    // Adding all client infos to _client vector in Server
    // _clients.emplace_back(clientFd, clientPort, clientIp, "", nickname);

    // authentificate client if password is set
    if (!_password.empty()) {
        handlePass(clientFd, "PASS " + _password, fds.size() - 1);
    } 
    else {
        std::string response = "Please provide a password using PASS <password>\n";
        send(clientFd, response.c_str(), response.size(), 0);
    }
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
