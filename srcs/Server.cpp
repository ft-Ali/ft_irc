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

    std::string target = message.substr(0, spacePos);
    std::string msg = message.substr(spacePos + 1);
    msg.erase(msg.find_last_not_of("\r\n") + 1);

    if (target[0] == '#' ) {
        Channel* channel = getChannelByName(target);
        Client* client = getClientByName(clientName);
        if (channel && channel->checkListMembers(client)) {
            std::cout << "Client " << clientName << " envoie un message au channel " << target << ": " << msg << std::endl;
            channel->broadcastMessage(client, msg);
        } else {
            std::string response = ":server_name 403 " + clientName + " " + target + " :No such channel\r\n";
            send(clientFd, response.c_str(), response.size(), 0);
        }
    } else {
        Client* targetClient = getClientByName(target);
        if (targetClient) {
        std::string response = ":" + clientName + " PRIVMSG " + target + " " + msg + "\r\n";
        send(targetClient->getFd(), response.c_str(), response.size(), 0);

        // std::string windowCommand = ":server_name NOTICE " + targetClient->getNickName() + " :/window goto " + targetClient->getNickName() + "\r\n";
        // send(targetClient->getFd(), windowCommand.c_str(), windowCommand.size(), 0);
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
    password.erase(password.find_last_not_of("\r\n") + 1);
    password.erase(password.find_last_not_of(" \t") + 1);

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
   if(newNickname.empty()) {
        std::string response = "ERROR: Nickname cannot be empty.\n";
        send(clientFd, response.c_str(), response.size(), 0);
        return;
    }
    for (std::map<int, std::string>::const_iterator it = _clientNicks.begin(); it != _clientNicks.end(); ++it) {
        if (it->second == newNickname) {
            _isConnected = false;
            return;
        }
    }
    _isConnected = true;
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
    // Ensure the client has already set a nickname
    if (_clientNicks.find(clientFd) == _clientNicks.end() || _clientNicks[clientFd].empty()) {
        std::string response = "ERROR: Please set a nickname first using the NICK command.\n";
        send(clientFd, response.c_str(), response.size(), 0);
        return;
    }
    //  const char* systemUser = getenv("USER"); // check doublon si doublon recheck le getenv() du systemUser qui vient de se connecter
    std::string username = _clientNicks[clientFd]; // si c'est different de systemUser alors tu prends le nickname
    _clientUsers[clientFd] = _clientNicks[clientFd];
    _clientRegistered[clientFd] = true;
    _clients[clientFd - 4]->setUsername(username);
    _clients[clientFd - 4]->setRegistered(true);
    std::string welcome = ":localhost 001 " + _clientNicks[clientFd] +
                          " :Welcome to the IRC Network, username: " + username + "\n";
    send(clientFd, welcome.c_str(), welcome.size(), 0);

    std::cout << "Client FD " << clientFd << " registered with username: " << username << std::endl;
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
        close(clientFd); // Fermer le FD
        _clientNicks.erase(clientFd); // Supprimer le nickname associé
        _clientUsers.erase(clientFd); // Supprimer le username associé
        _authenticatedClients.erase(clientFd); // Supprimer l'état d'authentification
        std::cout << "Client FD " << clientFd << " disconnected and cleaned up." << std::endl;
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
        }
        else if (line.find("NICK ") == 0) {
            handleNick(clientFd, line);
            if(_isConnected == false) {
                std::string response = "ERROR: Nickname already in use.\n";
                send(clientFd, response.c_str(), response.size(), 0);
                close(clientFd);
                _authenticatedClients.erase(clientFd);
                _clientNicks.erase(clientFd);
                _clientRegistered.erase(clientFd);
                fds.erase(fds.begin() + i);
                return;
            }
        } else if (line.find("USER ") == 0) {
            handleUser(clientFd);
        } else if (line.find("JOIN") == 0) {
            processJoin(clientName, line);
        } else if (line.find("TOPIC") == 0) {
            handleTopic(clientFd, line);
        } else if (line.find("PART") == 0) {
            processPart(client, line);
        } else if (line.find("PRIVMSG") == 0) {
            handlePrivMsg(line, clientFd);
        } else if (line.find("PING") == 0) {
            std::string pong = "PONG " + line.substr(5) + "\n";
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
    int clientFd = accept(_serSocketFd, (struct sockaddr*)&clientAddr, &clientAddrLen);
    if (clientFd == -1)
        throw(std::runtime_error("error: accept() failed"));

    char clientIp[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &clientAddr.sin_addr, clientIp, INET_ADDRSTRLEN);
    int clientPort = ntohs(clientAddr.sin_port);

    struct pollfd client;
    client.fd = clientFd;
    client.events = POLLIN;
    client.revents = 0;
    fds.push_back(client);

    for (std::map<int, std::string>::const_iterator it = _clientUsers.begin(); it != _clientUsers.end(); ++it) {
    // if (it->second == _clientUsers[clientFd]) {
    //         std::string response = "ERROR: Username already in use.\n";
    //         send(clientFd, response.c_str(), response.size(), 0);
    //         close(clientFd);
    //         fds.pop_back();
    //         return ;
    //     }
    }


    _clientNicks[clientFd] = "";
    _clientUsers[clientFd] = "";
    _clients.push_back(new Client(clientFd, clientPort, clientIp, "", ""));

    std::cout << "New connection: FD " << clientFd << " (" << clientIp << ":" << clientPort << ") "
         << std::endl;
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

Client *Server::getClientByFds(const int &clientFd){
	for (size_t i = 0; i < _clients.size(); ++i) {
        if (_clients[i]->getFd() == clientFd)
            return _clients[i];
    }
	return NULL;
}
