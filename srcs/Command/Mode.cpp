#include "../../inc/Server.hpp"

// MODE - Changer le mode du channel :
// — i : Définir/supprimer le canal sur invitation uniquement good
// — t : Définir/supprimer les restrictions de la commande TOPIC pour les opérateurs de canaux a check
// — k : Définir/supprimer la clé du canal (mot de passe) good
// — o : Donner/retirer le privilège de l’opérateur de canal
// — l : Définir/supprimer la limite d’utilisateurs pour le canal

void Server::manageMode(std::string &cmd, Client *client) {
    std::vector<std::string> cmdMode = splitArg(cmd, ' ');
    if (cmdMode.size() < 2) {
        sendClientResponse(client, ":[IRC] 461 " + client->getNickName() + " MODE :Not enough parameters\r\n");
        return;
    }
    Channel *channel = getChannelByName(cmdMode[1]);
    if (!channel) {
        return;
    }
    std::string mode = (cmdMode.size() > 2) ? cmdMode[2] : "";
    std::string param = (cmdMode.size() > 3) ? cmdMode[3] : "";
    if (mode.empty()) {
        std::string currentModes = ":" + client->getNickName() + " MODE " + channel->getName() + " :" + channel->getModes() + "\r\n";
        channel->broadcastInfoMessage(currentModes);
        return;
    }
    if(!channel->checkListMembers(client)){
        sendClientResponse(client, ":[IRC] 441 " + client->getNickName() + " MODE" + channel->getName() + " :They aren't on that channel\r\n");
        return;
    }
    if(channel->checkOperatorList(client)){ 
        handleModeActions(mode, param, client, channel);
        std::string updatedModes = ":" + client->getNickName() + " MODE " + channel->getName() + " :" + channel->getModes() + "\r\n";
        channel->broadcastInfoMessage(updatedModes);
    }
    else
        sendClientResponse(client, ":[IRC] 482 " + channel->getName() + " MODE :You're not channel operator\r\n");
}


void Server::handleModeActions(const std::string &mode, std::string &param, Client *client, Channel *channel) {
    char sign = mode[0];
    if (sign != '+' && sign != '-') {
        sendClientResponse(client, ":[IRC] 501 " + client->getNickName() + " MODE :Unknown MODE flag\r\n");
        return;
    }

    for (size_t i = 1; i < mode.size(); ++i) {
        char modeFlag = mode[i];
        switch (modeFlag) {
            case 'i':
            case 't':
                handleBasicMode(modeFlag, sign, client, channel);
                break;
            case 'k':
                handleKeyMode(sign, param, client, channel);
                break;
            case 'l':
                handleLimitMode(sign, param, client, channel);
                break;
            case 'o':
                handleOperatorMode(sign, param, client, channel);
                break;
            default:
                sendClientResponse(client, ":[IRC] 501 " + client->getNickName() + " MODE :Unknown MODE flag\r\n");
                return;
        }
    }
}

void Server::handleBasicMode(char mode, char sign, Client *client, Channel *channel) {
    (void)client;
    if (mode == 'i' && sign == '+') 
        channel->setInvitOnly(true);
    if (mode == 'i' && sign == '-') 
        channel->setInvitOnly(false);
    if (mode == 't' && sign == '+') 
        channel->setTopicMode(true);
    if (mode == 't' && sign == '-') 
        channel->setTopicMode(false);
    channel->addMode(mode, sign);
}

void Server::handleKeyMode(char sign, std::string &param, Client *client, Channel *channel) {
    if (sign == '+' && !param.empty()) {
        channel->setKey(param);
    } else if (sign == '-') {
        channel->undoKey();
    } else {
        sendClientResponse(client, ":[IRC] 461 " + client->getNickName() + " MODE :Key parameter required\r\n");
    }
    channel->addMode('k', sign);
}

void Server::handleLimitMode(char sign, const std::string &param, Client *client, Channel *channel) {
    if (sign == '+' && !param.empty()) {
        size_t limit = 0;
        std::stringstream ss(param);
        if (!(ss >> limit) || limit == 0 || limit > 500) {
            sendClientResponse(client, ":[IRC] 501 " + client->getNickName() + " MODE :Invalid channel size\r\n");
            return;
        }
        channel->setMaxMembers(limit);
    } else if (sign == '-') {
        channel->setMaxMembers(200);
    } else {
        sendClientResponse(client, ":[IRC] 461 " + client->getNickName() + " MODE :Limit parameter required\r\n");
    }
    channel->addMode('l', sign);
}

void Server::handleOperatorMode(char sign, std::string &param, Client *client, Channel *channel) {
    Client *targetClient = getClientByName(param);
    if (!targetClient) {
        return;
    }

    if (sign == '+' && !channel->checkOperatorList(targetClient)) {
        channel->setOperator(targetClient);
    } else if (sign == '-') {
        if (channel->getSizeOpeList() == 1) {
            sendClientResponse(client, ":[IRC] 485 " + client->getNickName() + " MODE :Cannot remove the last operator\r\n");
            return;
        }
        channel->removeOperator(targetClient);
    }
    channel->addMode('o', sign);
}

void sendClientResponse(Client *client, const std::string &response) {
    send(client->getFd(), response.c_str(), response.size(), 0);
}

