#include "../../inc/Server.hpp"
#include "../../inc/Channel.hpp"

void Server::cmdPart(const std::string &msg, std::vector<std::string> &args, Client *client) {
    std::string channelName = args[0];
    Channel* channel = getChannelByName(channelName);

    if (!client) {
        std::cout << "Error: Client is null.\n";
        return;
    }

    if (!channel) {
        std::cout << "Error: Channel " << channelName << " does not exist.\n";
        return;
    }

    if (!channel->checkListMembers(client)) {
        std::cout << "Error: Client is not a member of channel " << channelName << ".\n";
        return;
    }

    // Avant de supprimer, vérifier si le client est un membre
    if (std::find(client->getJoinedChannels().begin(), client->getJoinedChannels().end(), channel) != client->getJoinedChannels().end()) {
        std::cout << "Before removeJoinedChannel: " << client->getJoinedChannels().size() << " channels.\n";
        channel->leaveChannel(client);
        client->removeJoinedChannel(channel);
        std::cout << "Channel removed from client's joined channels.\n";
         std::cout << "Client has left channel " << channelName << " with message: " << msg << ".\n";
        std::cout << "After removeJoinedChannel: " << client->getJoinedChannels().size() << " channels.\n";
    } else {
        std::cerr << "Error: Client is not a member of channel " << channelName << ".\n";
    }

   
}



void Server::cmdPartMulti(const std::string &message, std::vector<std::string> &arg, Client *client) {
    // Split channel names by ',' and get client's current channels
    std::vector<std::string> channelNames = splitArg(arg[0], ',');
    std::vector<Channel*> clientChannels = client->getJoinedChannels();

    for (size_t i = 0; i < channelNames.size(); ++i) {
        // Get the channel by name
        Channel* channel = getChannelByName(channelNames[i]);

        // Check if the channel exists
        if (!channel) {
            std::cout << "Error: Channel " << channelNames[i] << " does not exist.\n";
            continue;
        }

        // Check if the client is a member of the channel
        if (!channel->checkListMembers(client)) {
            std::cout << "Error: Client is not in channel " << channelNames[i] << ".\n";
            continue;
        }

        // Remove client from channel
        channel->leaveChannel(client);

        // Remove channel from client's list of joined channels
        client->removeJoinedChannel(channel);

        // Verify removal
        const std::vector<Channel*>& updatedChannels = client->getJoinedChannels();
        if (std::find(updatedChannels.begin(), updatedChannels.end(), channel) == updatedChannels.end()) {
            std::cout << "Client successfully left channel " << channelNames[i] << " with message: " << message << ".\n";
        } else {
            std::cerr << "Error: Channel " << channelNames[i] << " still appears in client's joined channels.\n";
        }
    }
}

void Server::processPart(Client *client, std::string &command){

    std::string msg = "";

    size_t cmdIdx = command.find(' ',4);

    if(cmdIdx != std::string::npos)
        command = command.substr(cmdIdx +1);
    
    std::vector<std::string> arg = splitArg(command, ' ');
    if (arg.size() < 1) {
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