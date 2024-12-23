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

void Server::cmdPart(const std::string& msg, std::vector<std::string>& channelNames, Client* client) {
    std::vector<Channel*> clientChannels = client->getJoinedChannels();
    
    for (std::vector<std::string>::iterator it = channelNames.begin(); it != channelNames.end(); ++it) {
        Channel* channel = getChannelByName(*it);
        
        if (!channel) {
            std::cout << "Error: Channel " << *it << " does not exist.\n";
            continue;
        }
        if (!channel->checkListMembers(client)) {
            std::cout << "Error: Client is not in channel " << *it << ".\n";
            continue;
        }
         channel->leaveChannel(client);
        client->removeJoinedChannel(channel);
        // Vérifie si le client est opérateur et le retire si c'est le cas
        if (channel->checkOperatorList(client)) {
            channel->removeOperator(client);

            // Vérifie si le canal a des membres restants
            std::vector<Client*> members = channel->getMembers();
            if (!members.empty()) {
                // Le premier client de la liste devient le nouvel opérateur
                Client* newOperator = members.front();  // Tu peux remplacer `front()` par une logique spécifique si nécessaire.
                channel->setOperator(newOperator);  // Ajoute le nouvel opérateur
                std::cout << "New operator for channel " << channel->getName() << " is: " << newOperator->getNickName() << std::endl;
            } else {
                std::cout << "Channel " << channel->getName() << " has no members left, so no new operator.\n";
            }
        }
        
       

        const std::vector<Channel*>& updatedChannels = client->getJoinedChannels();
        bool removed = true;
        for (std::vector<Channel*>::const_iterator chIt = updatedChannels.begin(); chIt != updatedChannels.end(); ++chIt) {
            if (*chIt == channel) {
                removed = false;
                break;
            }
        }
        if (removed) {
            std::cout << "Client successfully left channel " << *it << " with message: " << msg << ".\n";
            std::cout << "Checking size of channel " << channel->getName() << ": " << channel->size() << std::endl;
        } else 
            std::cerr << "Error: Channel " << *it << " still appears in client's joined channels.\n";
        // sendCloseWindowCommand(client->getFd(), channel);  
        if (channel->size() == 0) {
            removeEmptyChannel(channel);  // Supprimer le canal si vide
        }
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
(void) channel;
    std::string response = "fd\r\n";
    if (send(clientFd, response.c_str(), response.size(), 0) < 0) {
        std::cerr << "Erreur : échec de l'envoi de la commande 'win close' au client.\n";
    } else {
        std::cout << "Commande 'win close' envoyée au client " << clientNickname << ".\n";
    }
    std::string flush = "fdfd\r\n";
    send(clientFd, flush.c_str(), flush.size(), 0);
}
