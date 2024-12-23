#include "../inc/Server.hpp"

void Server::handlePrivMsg(const std::string& line, int clientFd) {
    std::string clientName = getClientByFd(clientFd);  // Récupère le nom du client à partir de son fd
    size_t pos = line.find("PRIVMSG");
    std::string message = line.substr(pos + 8);
    size_t spacePos = message.find(' ');

    if (spacePos == std::string::npos) {
        std::string response = "ERROR: Invalid PRIVMSG format.\n";
        send(clientFd, response.c_str(), response.size(), 0);
        return;
    }

    std::string target = message.substr(0, spacePos);  // Nom du canal ou de l'utilisateur cible
    std::string msg = message.substr(spacePos + 1);  // Le message proprement dit
    msg.erase(msg.find_last_not_of("\r\n") + 1);  // Nettoie les caractères de fin de ligne

    if (target[0] == '#' ) {  // Si le message est destiné à un canal
        Channel* channel = getChannelByName(target);  // Récupère le canal par son nom
        Client* client = getClientByName(clientName);  // Récupère le client qui envoie le message
        if (channel && channel->checkListMembers(client)) {
            std::cout << "Client " << clientName << " envoie un message au channel " << target << ": " << msg << std::endl;
            channel->broadcastMessage(client, msg);  // Diffuse le message à tous les membres du canal
        } else {
            std::string response = ":server_name 403 " + clientName + " " + target + " :No such channel\r\n";
            send(clientFd, response.c_str(), response.size(), 0);
        }
    } else {  // Si le message est destiné à un utilisateur spécifique
        Client* targetClient = getClientByName(target);  // Récupère le client cible
        if (targetClient) {
            std::string response = ":" + clientName + " PRIVMSG " + target + " :" + msg + "\r\n";
            send(targetClient->getFd(), response.c_str(), response.size(), 0);
        } else {
            std::string response = ":server_name 401 " + clientName + " " + target + " :No such nick/channel\r\n";
            send(clientFd, response.c_str(), response.size(), 0);
        }
    }
}

// Function that handles the PASS command sent by the client.
// This command is used to authenticate the client with a password.
// - If the password is correct, the client is authenticated and can proceed with sending the NICK and USER commands.
// - If the password is incorrect, the connection is closed for this client.

// Parameters:
// - clientFd: The file descriptor of the client sending the PASS command.
// - message: The received message containing the password (the "PASS " command part is removed).
// - i: The index of the client in the fds list (used to remove the client from fds when disconnecting).

void Server::handlePass(int clientFd, const std::string& message, size_t i) {

    std::string password = message.substr(5);
    password.erase(password.find_last_not_of("\r\n") + 1); // Supprime '\r' et '\n' en fin de chaîne
    password.erase(password.find_last_not_of(" \t") + 1);  // Supprime les espaces et tabulations

    if (password.empty()) {
        std::string response = "ERROR: Password cannot be empty. Disconnecting.\n";
        send(clientFd, response.c_str(), response.size(), 0);
        close(clientFd);
        _authenticatedClients.erase(clientFd);
        _clientNicks.erase(clientFd);
        _clientRegistered.erase(clientFd);
        fds.erase(fds.begin() + i);
        return;
    }
    if (password == _password) {
        _authenticatedClients[clientFd] = true;
        std::string response = "Welcome! Use /quote USER to continue.\n";
        send(clientFd, response.c_str(), response.size(), 0);
        std::cout << "Client FD " << _clients[clientFd -4]->getUserName() << " authenticated successfully." << std::endl;
    } else if(!_authenticatedClients[clientFd]) {
        std::string response = "ERROR: Authentication failed. Disconnecting.\n";
        send(clientFd, response.c_str(), response.size(), 0);
        close(clientFd);
        _authenticatedClients.erase(clientFd);
        _clientNicks.erase(clientFd);
        _clientRegistered.erase(clientFd);
        fds.erase(fds.begin() + i);
        std::cout << "Client FD " << _clients[clientFd -4]->getUserName() << " failed authentication." << std::endl;
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
    _clients[clientFd -4]->setNickname(newNickname);
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
        std::string response = "USER cannot be empty\n";
        send(clientFd, response.c_str(), response.size(), 0);
        return;
    }
    // Automatically use system's username or predefined values for username
    const char* systemUser = getenv("USER");
    std::string username = systemUser ? systemUser : _clientNicks[clientFd]; // Set default if not available
    _clientUsers[clientFd] = username;
    _clientRegistered[clientFd] = true;
    _clients[clientFd -4]->setUsername(username);
    _clients[clientFd -4]->setRegistered(true);
    std::string welcome = ":localhost 001 " + _clientNicks[clientFd] + 
                          " :Welcome to the IRC Network, username: " + username + "\n";
    send(clientFd, welcome.c_str(), welcome.size(), 0);
}



Server* Server::instance = NULL; // initialize instance to NULL, (instance = global that point to actual server [ON])

void Server::closeServer() {
if (_serSocketFd != -1) {
    close(_serSocketFd);
    std::cout << "Server socket closed." << std::endl;
}

// Fermeture des descripteurs clients
for (size_t i = 0; i < fds.size(); ++i) {
    close(fds[i].fd);
    std::cout << "Client FD " << fds[i].fd << " closed." << std::endl;
}

// Libération des objets Client*
for (size_t i = 0; i < _clients.size(); ++i) {
    delete _clients[i];
}
_clients.clear();

// Libération des objets Channel*
for (size_t i = 0; i < _channels.size(); ++i) {
    delete _channels[i];
}
_channels.clear();

// Nettoyage des autres structures
_authenticatedClients.clear();
_clientRegistered.clear();
_clientUsers.clear();
_clientNicks.clear();

// Libération mémoire du vecteur pollfd
std::vector<struct pollfd>().swap(fds);

}

void Server::signalHandler(int signal) {
    if(signal == SIGQUIT) {
		std::cout << std::endl;
		std::cout << "SIGQUIT received. Closing server." << std::endl;
		instance->closeServer();
		// exit(0);
	}
	else if(signal == SIGINT) {
		std::cout << std::endl;
		std::cout << "SIGINT received. Closing server." << std::endl;
		instance->closeServer();
		// exit(0);
	}
}

void Server::handleCap(int clientFd, const std::string& message) {
    // Si le client n'est pas authentifié, ne répondre pas aux commandes CAP
    if (_authenticatedClients.find(clientFd) == _authenticatedClients.end() || !_authenticatedClients[clientFd]) {
        std::string response = "ERROR: Authentication required.\n";
        send(clientFd, response.c_str(), response.size(), 0);
        return;
    }

    if (message.find("CAP LS") == 0 && !_authenticatedClients[clientFd]) {
        std::string response = "CAP * LS :multi-prefix\n";
        send(clientFd, response.c_str(), response.size(), 0);
        std::cout << "Sent CAP LS response to client FD " << clientFd << std::endl;
    } 
    else if (message.find("CAP REQ") == 0) {
        // Le client demande une ou plusieurs capacités spécifiques
        size_t pos = message.find(":");
        std::string requestedCaps = message.substr(pos + 1);
        requestedCaps.erase(requestedCaps.find_last_not_of(" \r\n") + 1);

        if (requestedCaps == "multi-prefix") {
            std::string response = "CAP * ACK :multi-prefix\n";
            send(clientFd, response.c_str(), response.size(), 0);
            std::cout << "Acknowledged capabilities: " << requestedCaps << std::endl;
        } else {
            std::string response = "CAP * NAK :" + requestedCaps + "\n";
            send(clientFd, response.c_str(), response.size(), 0);
            std::cout << "Rejected capabilities: " << requestedCaps << std::endl;
        }
    } else if (message.find("CAP END") == 0) {
        std::string nickname = _clientNicks[clientFd];
        std::string welcomeMsg = ":localhost 001 " + nickname + " :Welcome to the IRC Network\n";
        send(clientFd, welcomeMsg.c_str(), welcomeMsg.size(), 0);
        std::cout << "Sent welcome message to client FD " << clientFd << std::endl;
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
    std::cout << "Received message (" << _clients[clientFd -4]->getUserName()<< ")" << message  << std::endl;

    std::istringstream stream(message);
    std::string line;
    while (std::getline(stream, line)) {
        line.erase(line.find_last_not_of("\r\n") + 1);
        if (line.empty()) 
            continue;
        std::string clientName = getClientByFd(clientFd);
        Client *client = getClientByName(clientName);
        if (_authenticatedClients.find(clientFd) == _authenticatedClients.end() || !_authenticatedClients[clientFd]) {
            if (line.find("PASS") == 0) {
                handlePass(clientFd, line, i);
            }
        }
        else if (line.find("CAP") == 0) {
            handleCap(clientFd, line);
        } else if (line.find("NICK ") == 0) {
            handleNick(clientFd, line);
        } else if (line.find("USER ") == 0) {
            handleUser(clientFd);
        } else if (line.find("JOIN") == 0) {
            processJoin(clientName, line);
        }else if (line.find("PART") == 0) {
            processPart(client, line);
        } else if (line.find("PRIVMSG") == 0) {
            handlePrivMsg(line, clientFd);
        } else if (line.find("PING") == 0) {
            std::string pong = "PONG " + line.substr(5) + "\n";
            int j=0;
              for (std::vector<Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it) {
                std::cout << "channel left : " << _channels[j]->getName() << std::endl;  
             
                j++;          }
            send(clientFd, pong.c_str(), pong.size(), 0);
        } else if (line.find("QUIT") == 0) {
            std::string response = "Goodbye!\n";
            send(clientFd, response.c_str(), response.size(), 0);
            close(clientFd);
            _authenticatedClients.erase(clientFd);
            _clientNicks.erase(clientFd);
            _clientRegistered.erase(clientFd);
            fds.erase(fds.begin() + i);
            return; 
        }
    }
}

void Server::handleNewConnection() {
     struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
      int clientFd = accept(_serSocketFd, (struct sockaddr*)&clientAddr,  &clientAddrLen); // Accept new connection // Accept new connection
    if (clientFd == -1) 
        throw(std::runtime_error("error: accept() failed"));
    char clientIp[INET_ADDRSTRLEN]; // Creates a char array to store the client’s IP address in human-readable form.
    inet_ntop(AF_INET, &clientAddr.sin_addr, clientIp, INET_ADDRSTRLEN); // Converts the binary representation of the client’s IP address into a human-readable string 
    int clientPort = ntohs(clientAddr.sin_port); // Converts the client’s port number from network byte order to host byte order and stores it in clientPort.
    struct pollfd client;
    client.fd = clientFd;
    client.events = POLLIN;
    client.revents = 0;
    fds.push_back(client);
    _clientNicks[clientFd] = "";
    _clientUsers[clientFd] = "";
    const char* systemUser = getenv("USER");
    if (!systemUser) 
        systemUser = getenv("LOGNAME");
    std::string nickname = systemUser ? systemUser : "anonymous_user";
    _clientNicks[clientFd] = nickname;
    _clients.push_back(new Client(clientFd, clientPort, clientIp, systemUser, nickname));
    // _clients[clientFd -4]->print();

}


void Server::serverLoop() {

	while (true) {
		if (poll(fds.data(), fds.size(), -1) == -1) // -1 -> infinite timeout
			throw(std::runtime_error("error: poll() failed"));

		for (size_t i = 0; i < fds.size(); ++i) { // loop through all fds (clients)
			if (fds[i].revents & POLLIN) { // POLLIN -> there is data to read
				
                if (fds[i].fd == _serSocketFd) { // new connection
					handleNewConnection();
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
		throw(std::runtime_error("listen() failed"));

	communication.fd = _serSocketFd;
	communication.events = POLLIN;
	communication.revents = 0;
	fds.push_back(communication);
	std::cout << "Server started on port " << _port << std::endl;
}


std::string Server::getClientByFd(const int &clientFd){
    std::string clientName;
    std::map<int , std::string>::iterator it = _clientNicks.find(clientFd);
        if(it != _clientNicks.end())
            clientName = it->second;
    return clientName;
}
