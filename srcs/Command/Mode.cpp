#include "../../inc/Server.hpp"
//  MODE - Changer le mode du channel :
// — i : Définir/supprimer le canal sur invitation uniquement
// — t : Définir/supprimer les restrictions de la commande TOPIC pour les opérateurs de canaux
// — k : Définir/supprimer la clé du canal (mot de passe)
// — o : Donner/retirer le privilège de l’opérateur de canal
// — l : Définir/supprimer la limite d’utilisateurs pour le canal

// void Channel::manageMode(std::string &mode){

//     if(mode)
//         return;
// }

void Channel::manageInvit(Client *client, Channel *channel, bool add){
    if (!channel || !client) 
        return;
    if(add){
        channel->setInvitOnly();
            if(client->isOperator() && !channel->checkWhiteList(client))
               channel->addToWhiteList(client);
        }
    else{
        channel->undoInvitOnly();
        clearVec(_whiteList, channel);
    }
}

void Channel::manageTopic(Channel *channel,bool add){
    if (!channel) 
        return;
    if(add){
        _editTopic = false;
    }
    else{
        _editTopic = true;
    }
}

void Channel::manageKey(Channel *channel, std::string &newKey, bool add){
    if (!channel) 
        return;
    if(!newKey.empty() && add){
        channel->undoKey();
        channel->setKey(newKey);
    }
    else{
        channel->undoKey();
    }
}

void Channel::manageOperator(Channel *channel, Client *client, bool add){
    if (!channel || !client) 
        return;
    if(add){
        channel->setOperator(client);
    }
    else{
        if (channel->getSizeVec(_operatorList) == 1) { 
            std::string response = ":server_name 485 " + client->getNickName() +
                                   " " + client->getNickName() + " :Cannot remove the last operator\r\n";
            send(client->getFd(), response.c_str(), response.size(), 0);
            return;
        }
        channel->removeOperator(client);}
}

void Channel::manageSizeChannel(Channel *channel, size_t size, bool add){
   if (!channel) 
        return;
    if(add){
        channel->setmaxMembers(size);
    }
    else
        channel->setmaxMembers(200);
}