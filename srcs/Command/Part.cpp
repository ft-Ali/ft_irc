#include "../../inc/Server.hpp"
#include "../../inc/Channel.hpp"

void    Server::cmdPart(const std::string &msg, std::vector<std::string> &args, Client *client){
    std::string channelName = args[0];

    Channel *channel = getChannelByName(channelName);
    if(!client){
        std::cout << "vide \n";
        return;}
    std::vector<Channel*> channels = client->getJoinedChannels();
    
    if(!channel || channels.empty()){
        std::cout << channelName << "does not exist\n";
        return ;
    }

    if(!channel->checkListMembers(client)){
        std::cout <<  "You are not in "<<channelName << std::endl;
        return ;
    }
    //send message
        // channel->broadcastMessage(client, "PART " + channelName + " :" + message);
    // client->removeJoinedChannel(channels, channel);
    client->removeJoinedChannel(_channels, channel);
    channel->removeClientList(_clients, client);
    // channel->leaveChannel(client);
    std::cout << "Client has left channel " << channelName << " with message: " << msg << ".\n";
    // if(_channels.empty())
    const std::vector<Channel*> chans = client->getJoinedChannels();

    std::cout << "Client left  " << chans.size() << " channel(s):\n";
    for (size_t i = 0; i < chans.size(); ++i) {
        std::cout << " - " << chans[i] << " (" << chans[i]->getName() << ")\n";
    }

}



void Server::cmdPartMulti(const std::string &message, std::vector<std::string> &arg, Client *client){

    std::vector<std::string> channelName = splitArg(arg[0],',');
    std::vector<Channel*> channels = client->getJoinedChannels();
   
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
    client->removeJoinedChannel(channels, channel);
    channel->leaveChannel(client);
    std::cout << "Client has left channel " << channelName[i] << " with message: " << message << ".\n";
    }

}

void Server::processPart(Client *client, std::string &command){

    std::string msg = "";
    std::cout<<"command full : '" << command << "'\n";

    size_t cmdIdx = command.find(' ',4);

    if(cmdIdx != std::string::npos)
        command = command.substr(cmdIdx +1);
    std::cout<<"cmd cut : '" <<command<< "'\n";
    
    std::vector<std::string> arg = splitArg(command, ' ');
    if (arg.size() < 2) {
        std::cout << "Error: No channel specified in the PART command.\n";
        return;
    }
    size_t msgIdx = command.find(':');
    if(msgIdx != std::string::npos){
        msg = command.substr(msgIdx +1);
    }
    
    if(arg.size() <= 2)
        cmdPart(msg,arg, client);
    else
        cmdPartMulti(msg,arg,client);
}

Client *Server::getClientByName(std::string &name){
	for (size_t i = 0; i < _clients.size(); ++i) {
        if (_clients[i]->getNickName() == name)
            return _clients[i];
    }
	return NULL;
}