#include "../../inc/Server.hpp"
#include "../../inc/Channel.hpp"
#include <sys/un.h>
void Server::processPart(Client* client, std::string& command) {
    if (!client) {
        std::cout << "Error: Client is null.\n";
        return;
    }
    std::string msg = "";
    size_t cmdIdx = command.find(' ', 4);
    if (cmdIdx != std::string::npos)
        command = command.substr(cmdIdx + 1);
    
    std::vector<std::string> arg = splitArg(command, ' ');
    if (arg.empty()) {
        std::cout << "Error: No channel specified in the PART command.\n";
        return;
    }
    size_t msgIdx = command.find(':');
    if (msgIdx != std::string::npos) {
        msg = command.substr(msgIdx + 1);
    }
    std::vector<std::string> channelNames = splitArg(arg[0], ',');
    cmdPart(msg, channelNames, client);
    
}

void Server::cmdPart(std::string& msg, std::vector<std::string>& channelNames, Client* client) {
    for (std::vector<std::string>::iterator it = channelNames.begin(); it != channelNames.end(); ++it) {
(void)msg;
        Channel* channel = getChannelByName(*it);
        if (!channel) {
            std::string response = ":server_name 403 " + client->getNickName() + " " + *it + " :No such channel\r\n";
            send(client->getFd(), response.c_str(), response.size(), 0);
            continue;
        }
        if (!channel->checkListMembers(client)) {
            std::string response = ":server_name 442 " + client->getNickName() + " " + *it + " :You're not on that channel\r\n";
            send(client->getFd(), response.c_str(), response.size(), 0);
            continue;
        }

        // Retire le client du canal
        if (channel->checkOperatorList(client)) {
            channel->removeOperator(client);
        }
        channel->leaveChannel(client);
        client->removeJoinedChannel(channel);

        // Vérifie si le canal est vide
        if (channel->size() == 0) {
            removeEmptyChannel(channel);
        }

        // Envoie la commande de fermeture de fenêtre
        sendCloseWindowCommand(client->getFd());
    }
}


// send msg to Server /widow prev pour return a la page precedente
void Server::removeEmptyChannel(Channel* channel) {
    std::cout << "Checking size of channel " << channel->getName() << ": " << channel->size() << std::endl; 
    if (channel->size() == 0) {
        std::vector<Channel*>::iterator it = std::find(_channels.begin(), _channels.end(), channel);
        if (it != _channels.end()) {
            _channels.erase(it);
            std::cout << "Channel " << channel->getName() << " has been removed from server's channels list." << std::endl;
        }
    }
}

Client *Server::getClientByName(std::string &name){
	for (size_t i = 0; i < _clients.size(); ++i) {
        if (_clients[i]->getNickName() == name)
            return _clients[i];
    }
	return NULL;
}

void Server::sendCloseWindowCommand(int clientFd) {
    std::string clientNickname = getClientByFd(clientFd);
    if (clientNickname.empty()) {
        std::cerr << "Erreur : Impossible de trouver le client pour FD " << clientFd << ".\n";
        return;
    }

    // Ajoute un préfixe "COMMAND" pour que le script IRSSI le reconnaisse
  std::string response = ":SERVER_COMMAND /window close\r\n";
send(clientFd, response.c_str(), response.size(), 0);
}
