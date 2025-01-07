#include "../../inc/Channel.hpp"
#include "../../inc/Server.hpp"

void Server::handleQuit(int clientFd, Client *client) {
    for (size_t i = 0; i < _channels.size(); ++i) {
        if (_channels[i]->checkListMembers(client)) {
            _channels[i]->removeMember(client);
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
    if(_clients[clientFd-4])
        delete []_clients[clientFd-4];
    std::string response = "Goodbye!\n";
    send(clientFd, response.c_str(), response.size(), 0);
   closeClient(clientFd);
}

void Server::removePollFd(int clientFd) {
    for (std::vector<pollfd>::iterator it = fds.begin(); it != fds.end(); ++it) {
        if (it->fd == clientFd) {
        std::cout << " fd" << it->fd << std::endl;
            fds.erase(it); 
            break;
        }
    }
}

void Server::closeClient(int clientFd) {
    close(clientFd);
    removePollFd(clientFd);
}
