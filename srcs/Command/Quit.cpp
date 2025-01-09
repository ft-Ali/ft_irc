#include "../../inc/Channel.hpp"
#include "../../inc/Server.hpp"

void Server::handleQuit(int clientFd, Client *client, bool free) {
    for (size_t i = 0; i < _channels.size(); ++i) {
        if (_channels[i]->checkListMembers(client)) {
            std::string name = _channels[i]->getName();
            processPart(client, name);
        }
    }

    std::vector<Client*>::iterator it = std::find(_clients.begin(), _clients.end(), client);
    if (it != _clients.end()) {
        delete *it;
        _clients.erase(it);
    }

    _authenticatedClients.erase(clientFd);
    _clientRegistered.erase(clientFd);
    _clientUsers.erase(clientFd);
    _clientNicks.erase(clientFd);

    // std::string response = "Goodbye!\n";
    // send(clientFd, response.c_str(), response.size(), 0);
    if (free)
        closeClient(clientFd);
}


void Server::removePollFd(int clientFd) {
    for (std::vector<pollfd>::iterator it = fds.begin(); it != fds.end(); ++it) {
        if (it->fd == clientFd) {
            fds.erase(it); 
            break;
        }
    }
}

void Server::closeClient(int clientFd) {
    close(clientFd);
    removePollFd(clientFd);
}
