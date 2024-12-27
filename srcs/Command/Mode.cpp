#include "../../inc/Server.hpp"
//  MODE - Changer le mode du channel :
// — i : Définir/supprimer le canal sur invitation uniquement
// — t : Définir/supprimer les restrictions de la commande TOPIC pour les opérateurs de canaux
// — k : Définir/supprimer la clé du canal (mot de passe)
// — o : Donner/retirer le privilège de l’opérateur de canal
// — l : Définir/supprimer la limite d’utilisateurs pour le canal

// void Server::manageMode(std::string &cmd, Client *client){

//     std::vector<std::string> cmdMode = splitArg(cmd, ' '); 

//     Channel *channel = getChannelByName(cmdMode[1]);
//     std::string mode = cmdMode[2];
  
//        for(size_t i = 1; i < mode.size();++i){
//         char sign = mode[0];
//         char modeFound;
//         modeFound = mode[i];

//         if(!cmdMode[3].empty()){
//             if(modeFound = 'o'){
//                 Client *newClient = getClientByName(cmdMode[3]);
//                 if(sign = '+')
//                     channel->manageOperator(channel, newClient, true);
//                 if(sign = '-')
//                     channel->manageOperator(channel, newClient, false); 
//             }
//             if(modeFound = 'l'){
//                 std::stringstream ss(cmdMode[3]);
//                 size_t value = 0;
//                 ss>>value;
//                 if(value > 500){
//                     std::string response = ":server_name 501 " + client->getNickName()+ " " + channel->getName() + " :Cannot join channel(+l)\r\n";
//                     send(client->getFd(), response.c_str(), response.size(), 0);
//                     continue;
//                 }
//                 if(sign = '+')
//                     channel->manageSizeChannel(channel, value, true);
//                 if(sign = '-')
//                     channel->manageSizeChannel(channel, value, false);
//             }
        
//         }
//     }
//         return;
// }

// void Channel::whichMode(char mode, char sign, Client *client, Channel *channel, std::string arg){
   
//     switch (mode)
//     {
//     case 'i' :
//         if(sign == '+')
//             manageInvit(client, channel, true);
//         else if(sign == '-')
//             manageInvit(client,channel,false);
//         break;
//     case 't' :
//         if(sign == '+')
//             manageTopic(channel, true);
//         else if(sign == '-')
//             manageTopic(channel,false);
//         break;
//     case 'k' :
//        if(sign == '+')
//             manageKey(channel,arg, true);
//         else if(sign == '-')
//             manageKey(channel,arg,false);
//         break;
//     case 'o' :
//         break;
//     case 'l' :
//         break;
//     default:
//         std::string response = ":server_name 501 " + client->getNickName()+ " " + channel->getName() + " :Unknown MODE flag\r\n";
//         send(client->getFd(), response.c_str(), response.size(), 0);
//         break;
//     }
//     channel->addMode(mode);
// }

// void Channel::manageInvit(Client *client, Channel *channel, bool add){
//     if (!channel || !client) 
//         return;
//     if(add){
//         channel->setInvitOnly();
//             if(channel->checkOperatorList(client) && !channel->checkWhiteList(client))
//                channel->addToWhiteList(client);
//         }
//     else{
//         channel->undoInvitOnly();
//         clearVec(_whiteList, channel);
//     }
// }

// void Channel::manageTopic(Channel *channel,bool add){
//     if (!channel) 
//         return;
//     if(add){
//         _editTopic = false;
//     }
//     else{
//         _editTopic = true;
//     }
// }

// void Channel::manageKey(Channel *channel, std::string &newKey, bool add){
//     if (!channel) 
//         return;
//     if(!newKey.empty() && add){
//         channel->undoKey();
//         channel->setKey(newKey);
//     }
//     else{
//         channel->undoKey();
//     }
// }

// void Channel::manageOperator(Channel *channel, Client *client, bool add){
//     if (!channel || !client) 
//         return;
//     if(add){
//         channel->setOperator(client);
//     }
//     else{
//         if (channel->getSizeVec(_operatorList) == 1) { 
//             std::string response = ":server_name 485 " + client->getNickName() +
//                                    " " + client->getNickName() + " :Cannot remove the last operator\r\n";
//             send(client->getFd(), response.c_str(), response.size(), 0);
//             return;
//         }
//         channel->removeOperator(client);
//     }
// }

// void Channel::manageSizeChannel(Channel *channel, size_t size, bool add){
//    if (!channel) 
//         return;
//     if(add){
//         channel->setmaxMembers(size);
//     }
//     else
//         channel->setmaxMembers(200);
// }