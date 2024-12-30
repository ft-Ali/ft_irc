#include "../../inc/Channel.hpp"
#include "../../inc/Server.hpp"

//kick <canal> <client> : message <optionnal>
void Server::handleKick(Client *client, std::string command){

    std::vector<std::string> cmd = splitArg(command ,' ');
    if (cmd.size() < 2) {
        sendClientResponse(client, ":server_name 461 KICK :Not enough parameters\r\n");
        return;
    }
    std::string targetName = (cmd.size() > 2) ? cmd[2] : "";
    std::string message;
    for(size_t i = 2 ; i < cmd.size(); ++i){
        if(!cmd[i].find(':')){
            message += cmd[i];
            message+= ' ';
        }
    }

    Channel *channel = getChannelByName(cmd[1]);
    if (!channel) {
        sendClientResponse(client, ":server_name 403 " + cmd[1] + " :No such channel\r\n");
        return;
    }
    Client* target = getClientByName(targetName);
    if (!target || !channel->checkListMembers(target)) {
        sendClientResponse(client, ":server_name 441 " + targetName + " " + channel->getName() + " :They aren't on that channel\r\n");
        return;
    }

    if (!channel->checkOperatorList(client)) {
        sendClientResponse(client, ":server_name 482 " + channel->getName() + " :You're not channel operator\r\n");
        return;
    }
    
   std::string response = ": KICK from " + channel->getName() + " by " + client->getNickName();
    if (!message.empty()) {
        response += " :" + message;
    }
    response += "\r\n";

    channel->broadcastMessage(client, response);

    sendClientResponse(target, response);
    channel->broadcastMessage(client, response);
    channel->removeMember(target);
}