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
    std::vector<Channel*> clientChannels = client->getJoinedChannels();

    for (std::vector<std::string>::iterator it = channelNames.begin(); it != channelNames.end(); ++it) {

        Channel* channel = getChannelByName(*it);
        if (!channel) {
            std::cout << "Erreur : le canal " << channel->getName() << " n'existe pas.\n";
            continue;
        }
        if (!channel->checkListMembers(client)) {
            std::cout << "Erreur : le client n'est pas dans le canal " << channel->getName() << ".\n";
            continue;
        }

        if (channel->checkOperatorList(client)) {
            channel->removeOperator(client);
        }
        channel->leaveChannel(client);
        client->removeJoinedChannel(channel);

        // Vérifier si le canal est vide
        if (channel->size() == 0) {
            removeEmptyChannel(channel);
        }

        // Message de confirmation
        std::string partMessage = "Client " + client->getNickName() + " a quitté le canal " + channel->getName() + " avec le message : " + msg + "\n";
        std::cout << partMessage;
// std::this_thread::sleep_for(std::chrono::seconds(1)); // Attente d'une 
std::string response = ":server_name NOTICE " + client->getNickName() + " :/window close\r\n";
send(client->getFd(), response.c_str(), response.size(), 0);

    }
}



// send msg to Server /widow prev pour return a la page precedente
void Server::removeEmptyChannel(Channel* channel) {
    std::cout << "Checking size of channel " << channel->getName() << ": " << channel->size() << std::endl; 
    if (channel->size() == 0) {
        // Trouver le canal dans le vecteur _channels et le supprimer
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

void Server::sendCloseWindowCommand(int clientFd, Channel *channel) {
    std::string clientNickname = getClientByFd(clientFd);
    if (clientNickname.empty()) {
        std::cerr << "Erreur : Impossible de trouver le client pour FD " << clientFd << ".\n";
        return;
    }
    (void)channel;
std::string response = ":server_name NOTICE " + clientNickname + " :/window close\r\n";
send(clientFd, response.c_str(), response.size(), 0);

    std::string flush = "\r\n";
    send(clientFd, flush.c_str(), flush.size(), 0);
}
