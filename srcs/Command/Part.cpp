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
    
    // Parcours des canaux spécifiés dans la commande PART
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
        
        channel->leaveChannel(client);
        client->removeJoinedChannel(channel);

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
        if (channel->size() == 0) {
            removeEmptyChannel(channel);  // Supprimer le canal si vide
        }
    }
    // sendCommandToIrssi("/window prev");  // Remettre la fenêtre précédente
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

void Server::sendCommandToIrssi(const std::string& command) {
    const char* socket_path = "/tmp/irssi_socket";
    struct sockaddr_un addr;
    int sock = socket(AF_UNIX, SOCK_STREAM, 0);

    if (sock == -1) {
        std::cerr << "Error creating socket" << std::endl;
        return;
    }

    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);

    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        std::cerr << "Error connecting to socket" << std::endl;
        close(sock);
        return;
    }

    send(sock, command.c_str(), command.length(), 0);
    close(sock);
}