#include "../../inc/Channel.hpp"
#include "../../inc/Server.hpp"

//invit <client> <canal>
void Server::handleInvit(Client *client, std::string command){
std ::cout << "nous " << command << std::endl;

    std::vector<std::string> cmd = splitArg(command ,' ');
    if (cmd.size() < 3) {
        sendClientResponse(client, ":server_name 461 INVIT :Not enough parameters\r\n");
        return;
    }
    std::string targetName = (cmd.size() > 2) ? cmd[1] : "";

    Channel *channel = getChannelByName(cmd[1]);
    Client* target = getClientByName(targetName);
    if (!channel) {
    sendClientResponse(client, ":server_name 403 " + cmd[1] + " :No such channel\r\n");
    return;
    }
    if (!target) {
        sendClientResponse(client, ":server_name 401 " + targetName + " :No such nick\r\n");
        return;
    }
    if (!channel->checkOperatorList(client)) {
        sendClientResponse(client, ":server_name 482 " + channel->getName() + " :You're not channel operator\r\n");
        return;
    }
    if(channel->checkListMembers(target)){
        sendClientResponse(client, ":server_name 443 "+ target->getNickName() + " :is already on channel" + channel->getName() + "\r\n");
        return;
    }
    if (!channel->getInvitOnly()) {
        sendClientResponse(client, ":server_name 403 " + channel->getName() + " :Channel is not invite-only\r\n");
        return;
    }
    std::string response = ":server " + client->getNickName() + " INVIT " + target->getNickName() + " :" + channel->getName() + "\r\n";
    sendClientResponse(target, response);
    channel->addToWhiteList(target);
std ::cout << "nous " << command << std::endl;
}