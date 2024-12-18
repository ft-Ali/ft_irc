#include "../../inc/Channel.hpp"
#include "../../inc/Server.hpp"

void    Server::cmdPart(const std::string &command, Client *client){
    

    std::vector<std::string> args = splitArg(command, ' ');
    if(command.size() < 2){
        cmdPartAll(command,client);
    }
    std::string channelName = args[1];
    std::string message = "";

    size_t msgIdx = command.find(':');
    if(msgIdx != std::string::npos){
        message = command.substr(msgIdx +1);
    }

    Channel *channel = getChannelByName(channelName);
    if(!channel){
        std::cout << channelName << "does not exist\n";
        return ;
    }

    if(!channel->checkListMembers(client)){
        std::cout <<  "You are not in "<<channelName << std::endl;
        return ;
    }
    //send message
        // channel->broadcastMessage(client, "PART " + channelName + " :" + message);
        // client->removeChannel(channel)

    channel->leaveChannel(client);
    std::cout << "Client has left channel " << channelName << " with message: " << message << ".\n";

}

void Server::processPart(Client *client, std::string &command){
    
    std::string msg = "";
    std::vector<std::string> arg = splitArg(command, ' ');
    if (arg.size() < 2) {
        std::cout << "Error: No channel specified in the PART command.\n";
        return;
    }

    size_t msgIdx = command.find(':');
    if(msgIdx != std::string::npos){
        msg = command.substr(msgIdx +1);
    }
    
    if(!arg.size() <= 3)
        cmdPart(arg, client);
}

void Server::cmdPartMulti(const std::string &message, std::vector<std::string> &arg, Client *client){

    std::vector<std::string> channelName = splitArg(arg[1],',');
   
    for(size_t i = 0; i < channelName.size(); ++i){

          Channel *channel = getChannelByName(channelName[i]);
        if(!channel){
            std::cout << channelName[i] << "does not exist\n";
            continue ;
        }
         if(!channel->checkListMembers(client)){
        std::cout <<  "You are not in "<<channelName[i] << std::endl;
        continue;
    }
    //send message
        // channel->broadcastMessage(client, "PART " + channelName + " :" + message);
        // client->removeChannel(channel)

    channel->leaveChannel(client);
    std::cout << "Client has left channel " << channelName[i] << " with message: " << message << ".\n";
    }

}
//attente du vector client
// void Server::cmdPartAll(Client* client) {
//     const std::vector<Channel*> channels = client->getChannels();

//     if (channels.empty()) {
//         std::cout << "Error: You are not in any channels.\n";
//         return;
//     }

//     for (size_t i = 0; i < channels.size(); ++i) {
//         Channel* channel = channels[i];
//         channel->removeMember(client);
//         // client->removeChannel(channel)
//         std::cout << "Client has left channel " << channel->getName() << ".\n";
//     }
// }

Client *Server::getClientByName(std::string &name){
	
	for (size_t i = 0; i < _clients.size(); ++i) {
        if (_clients[i]->getName() == name)
            return _clients[i];
    }
	return NULL;
}