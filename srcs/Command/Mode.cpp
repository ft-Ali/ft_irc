#include "../../inc/Server.hpp"
//  MODE - Changer le mode du channel :
// — i : Définir/supprimer le canal sur invitation uniquement
// — t : Définir/supprimer les restrictions de la commande TOPIC pour les opérateurs de canaux
// — k : Définir/supprimer la clé du canal (mot de passe)
// — o : Donner/retirer le privilège de l’opérateur de canal
// — l : Définir/supprimer la limite d’utilisateurs pour le canal

void Server::manageMode(std::string &cmd, Client *client){

    std::vector<std::string> cmdMode = splitArg(cmd, ' '); 
    int i = 0;
    std::string mode;
    std::string param;

    Channel *channel = getChannelByName(cmdMode[1]);
    if(!channel){
        std::cout << "yuyuuyuy \n";
        return ;
    }
    for(std::vector<std::string>::iterator it = cmdMode.begin(); it != cmdMode.end();++it){
        if(i==2)
            mode = cmdMode[2];
        if(i==3)
            param = cmdMode[3];
    i++;
    }
    if(mode.empty()){
        std::cout << "MODE " << channel->getName() << ": " << channel->getModes() << std::endl;
    // return ;    
    }
       for(size_t i = 1; i < mode.size();++i){
                std::cout << "yuyuuyuyaaaa \n";

        char sign = mode[0];
        char modeFound;
        modeFound = mode[i];

        if(param.empty()){
            channel->whichMode(modeFound, sign, client, channel);
             if(sign == '-' && modeFound == 'l')
                    channel->manageSizeChannel(channel, 0, false);
            }
        else if(!param.empty()){
            if(modeFound == '+' && sign == 'k')
                channel->manageKey(channel, param, true);
            if(modeFound == 'l'){
                std::stringstream ss(param);
                size_t value = 0;
                ss>>value;
                if(value > 500){
                    std::string response = ":server_name 501 " + client->getNickName()+ " " + channel->getName() + " :Cannot join channel(+l)\r\n";
                    send(client->getFd(), response.c_str(), response.size(), 0);
                }
                else if(sign == '+')
                    channel->manageSizeChannel(channel, value, true);
            }
            if(modeFound == 'o'){
                Client *newClient = getClientByName(param);
                if(!newClient)
                    continue;
                if(sign == '+')
                    channel->manageOperator(channel, newClient, true);
                if(sign == '-')
                    channel->manageOperator(channel, newClient, false); 
            }
        }
    channel->addMode(modeFound, sign);
    }
}

void Channel::whichMode(char mode, char sign, Client *client, Channel *channel){
   
    switch (mode)
    {
    case 'i' :
        if(sign == '+')
            manageInvit(client, channel, true);
        else if(sign == '-')
            manageInvit(client,channel,false);
        break;
    case 't' :
        if(sign == '+')
            manageTopic(channel, true);
        else if(sign == '-')
            manageTopic(channel,false);
        break;
    case 'k' :
        if(sign == '-')
            manageKey(channel,"",false);
        break;
    case 'o' :
        break;
    case 'l' :
        break;
    default:
        std::string response = ":server_name 501 " + client->getNickName()+ " " + channel->getName() + " :Unknown MODE flag\r\n";
        send(client->getFd(), response.c_str(), response.size(), 0);
        return ;
    }
    channel->addMode(mode, sign);
}

void Channel::manageInvit(Client *client, Channel *channel, bool add){
    if (!channel || !client) 
        return;
    if(add){
        channel->setInvitOnly();
            if(channel->checkOperatorList(client) && !channel->checkWhiteList(client))
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

void Channel::manageKey(Channel *channel, std::string newKey, bool add){
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
        channel->removeOperator(client);
    }
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