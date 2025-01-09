#include "../inc/Server.hpp"

void Server::handlePrivMsg(const std::string& line, int clientFd) {
    std::string clientName = getClientByFd(clientFd);
    std::vector<std::string> cmd = splitArg(line, ' ');
    if(cmd.size() < 3){
        std::string rep = ":[IRC] 461  :Not enough parameters\r\n";
        send(clientFd, rep.c_str(), rep.size(), 0);
        return;
    }

    std::string target = cmd[1];
    std::string msg ;
    for(size_t i = 2; i < cmd.size();++i){
        msg += cmd[i] +' ';   
    }
    if (target[0] == '#' ) {
        Channel* channel = getChannelByName(target);
        Client* client = getClientByName(clientName);
        if (channel && channel->checkListMembers(client)) {
            if (channel->checkOperatorList(client)) 
                clientName = "@" + clientName;
            channel->broadcastMessage(client, msg);
        } else {
            std::string response = ":server_name 403 " + clientName + " " + target + " :No such channel\r\n";
            send(clientFd, response.c_str(), response.size(), 0);
        }
    }else {  
    Client* targetClient = getClientByName(target);
    if (targetClient) {
        std::string response = ":" + clientName + " PRIVMSG " + target + " " + msg + "\r\n";
        send(targetClient->getFd(), response.c_str(), response.size(), 0);
    } else {
        Channel* channel = getChannelByName(target);
        if (channel) {
            std::string response = ":" + clientName + " PRIVMSG " + target + " " + msg + "\r\n";
            channel->broadcastMessage(targetClient, msg);
        } else {
            std::string response = ":server_name 401 " + clientName + " " + target + " :No such nick/channel\r\n";
            send(clientFd, response.c_str(), response.size(), 0);
        }
    }
}

}


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
    } else if(!_authenticatedClients[clientFd]) {
        std::string response = "ERROR: Authentication failed. Disconnecting.\n";
        send(clientFd, response.c_str(), response.size(), 0);
        close(clientFd);
        _authenticatedClients.erase(clientFd);
        _clientNicks.erase(clientFd);
        _clientRegistered.erase(clientFd);
        fds.erase(fds.begin() + i);
    }
}

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
    size_t i;
    for(i = 0; i < _clients.size() -1;++i){
        if(_clients[i]->getFd() == clientFd)
            break ;
    }
    _clients[i]->setNickname(newNickname);
    std::string response = ":" + oldNickname + " NICK " + newNickname + "\n";
    send(clientFd, response.c_str(), response.size(), 0);
    std::cout << "Client FD " << clientFd << " changed nickname from "
              << oldNickname << " to " << newNickname << std::endl;
}

void Server::handleUser(int clientFd) {
    if (_clientNicks.find(clientFd) == _clientNicks.end() || _clientNicks[clientFd].empty()) {
        std::string response = "ERROR: Please set a nickname first using the NICK command.\n";
        send(clientFd, response.c_str(), response.size(), 0);
        return;
    }
    std::string username = _clientNicks[clientFd]; 
    _clientUsers[clientFd] = _clientNicks[clientFd];
    _clientRegistered[clientFd] = true;
    size_t i;
    for(i = 0; i < _clients.size() -1;++i){
    }
    _clients[i]->setUsername(username);
    _clients[i]->setRegistered(true);
    std::string welcome = ":localhost 001 " + _clientNicks[clientFd] +
                          " :Welcome to the IRC Network, username: " + username + "\n";
    send(clientFd, welcome.c_str(), welcome.size(), 0);
    std::cout << "Client FD " << clientFd << " registered with username: " << username << std::endl;
}

Server* Server::instance = NULL;

void Server::closeServer() {
if (_serSocketFd != -1) {
    close(_serSocketFd);
    std::cout << "Server socket closed." << std::endl;
}
for (size_t i = 0; i < fds.size(); ++i) {
    close(fds[i].fd);
}
for (size_t i = 0; i < _clients.size(); ++i) 
    delete _clients[i];
_clients.clear();
if(!_channels.empty()){ 
    for (size_t i = 0; i < _channels.size(); ++i) {
        if (_channels[i] != NULL) { 
            delete _channels[i]; 
            _channels[i] = NULL; 
        }
    }
}
_channels.clear();
_authenticatedClients.clear();
_clientRegistered.clear();
_clientUsers.clear();
_clientNicks.clear();
std::vector<struct pollfd>().swap(fds);
}

void Server::signalHandler(int signal) {
    if(signal == SIGQUIT) {
		std::cout << std::endl;
		std::cout << "SIGQUIT received. Closing server." << std::endl;
		instance->closeServer();
	}
	else if(signal == SIGINT) {
		std::cout << std::endl;
		std::cout << "SIGINT received. Closing server." << std::endl;
		instance->closeServer();
	}
}

void Server::handleCap(int clientFd, const std::string& message) {
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

void Server::handleClientMessage(int index) {
 int clientFd = fds[index].fd;
   
    char buffer[1024];

    int ret = recv(clientFd, buffer, sizeof(buffer) - 1, 0);
    if (ret <= 0) {
        disconnectClient(clientFd, index);
        return;
    }

    buffer[ret] = '\0'; 
    std::string message(buffer);
    std::cout << message << std::endl;

    std::istringstream stream(message);
    std::string line;

    while (std::getline(stream, line)) {
        line.erase(line.find_last_not_of("\r\n") + 1);
        if (line.empty())
            continue;

        Client *client = getClientByFds(clientFd);
        if (!client) {
            std::cerr << "Erreur : client not found " << clientFd << std::endl;
            continue;
        }
        if (_authenticatedClients.find(clientFd) == _authenticatedClients.end() || !_authenticatedClients[clientFd]) {
            if (line.find("PASS") == 0) {
                handlePass(clientFd, line, index);
            }
        } else {
            if (line.find("CAP") == 0) {
                handleCap(clientFd, line);
            } else if (line.find("NICK ") == 0) {
                handleNick(clientFd, line);
                if (!_isConnected) {
                    sendError(clientFd, "ERROR: Nickname already in use.");
                    disconnectClient(clientFd, index);
                    return;
                }
            } else if (line.find("USER ") == 0) {
                handleUser(clientFd);
            } else if (line.find("JOIN") == 0) {
                processJoin(client->getNickName(), line);
            } else if (line.find("TOPIC") == 0) {
                handleTopic(clientFd, line);
            } else if (line.find("PART") == 0) {
                processPart(client, line);
            } else if (line.find("PRIVMSG") == 0) {
                handlePrivMsg(line, clientFd);
            } else if (line.find("MODE") == 0) {
                manageMode(line, client);
            } else if (line.find("KICK") == 0) {
                handleKick(client, line);
            } else if (line.find("INVITE") == 0) {
                handleInvit(client, line);
            } else if (line.find("PING") == 0) {
                handlePing(clientFd, line);
            } else if (line.find("QUIT") == 0) {
                handleQuit(clientFd, client, true);
            } 
           else {
                handleUnknownCommand(client, line);
            }
        }
    }
}

void Server::disconnectClient(int clientFd, size_t index) {
    std::cout << "Client FD " << clientFd << " disconnected" << std::endl;
  for (size_t i = 0; i < _channels.size(); ++i) {
        Client *client = getClientByFds(clientFd);
        if(!client)
            continue;
        if (_channels[i]->checkListMembers(client)) {
            std::string name = _channels[i]->getName();
            std::cout << "Removing client from channel: " << name << std::endl;
            processPart(client, name);
        }
    }
    close(clientFd);
    _clientNicks.erase(clientFd);
    _clientUsers.erase(clientFd);
    _authenticatedClients.erase(clientFd);
    fds.erase(fds.begin() + index);
    for (std::vector<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        if ((*it)->getFd() == clientFd) {
            delete *it;
            _clients.erase(it);
            break;
        }
    }
}

void Server::handleUnknownCommand(Client *client, const std::string &line) {
    std::string response = ":server_name 421 " + client->getNickName() + " '" + line + "' :Unknown command\r\n";
    send(client->getFd(), response.c_str(), response.size(), 0);
}

void Server::sendError(int clientFd, const std::string &message) {
    std::string response = message + "\r\n";
    send(clientFd, response.c_str(), response.size(), 0);
}

void Server::handlePing(int clientFd, const std::string &line) {
    std::string pong = "PONG " + line.substr(4) + "\r\n";
    send(clientFd, pong.c_str(), pong.size(), 0);
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
    _clientNicks[clientFd] = "";
    _clientUsers[clientFd] = "";
    _clients.push_back(new Client(clientFd, clientPort, clientIp, "", ""));
    std::cout << "New connection: FD " << clientFd << " (" << clientIp << ":" << clientPort << ") "
         << std::endl;
}


void Server::serverLoop() {

	while (true) {
		if (poll(fds.data(), fds.size(), -1) == -1)
			throw(std::runtime_error("Server closed"));

		for (size_t i = 0; i < fds.size(); ++i) {
			if (fds[i].revents & POLLIN) { 

                if (fds[i].fd == _serSocketFd) {
					handleNewConnection();
				} else {
					handleClientMessage(i);
				}
			}
		}
	}
}

void Server::serverInit() {

	struct sockaddr_in 	addr;
	struct pollfd		communication;

	addr.sin_family = AF_INET;
	addr.sin_port = htons(_port);
	addr.sin_addr.s_addr = INADDR_ANY;

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

