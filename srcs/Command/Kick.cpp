#include "../../inc/Channel.hpp"
#include "../../inc/Server.hpp"

//kick <canal> <client> : message <optionnal>
void Server::handleKick(Client *client, std::string command){

    std::vector<std::string> cmd = splitArg(command ,' ');
    if (cmd.size() < 3) {
        sendClientResponse(client, ":[IRC] 461 " + client->getNickName() + " KICK :Not enough parameters\r\n");
        return;
    }
    std::string targetName = (cmd.size() > 2) ? cmd[2] : "";
    std::string message;
    for (size_t i = 2; i < cmd.size(); ++i) {
        if (i == 2 && cmd[i][0] == ':') {
            message = cmd[i].substr(1);
        } else {
            message += cmd[i] + ' ';
        }
    }
    Channel *channel = getChannelByName(cmd[1]);
    if (!channel) {
        sendClientResponse(client, ":[IRC] 403 " + cmd[1] + " KICK :No such channel\r\n");
        return;
    }
    Client* target = getClientByName(targetName);
    if (!target || !channel->checkListMembers(target)) {
        sendClientResponse(client, ":[IRC] 441 " + targetName + " " + channel->getName() + " KICK :They aren't on that channel\r\n");
        return;
    }

    if (!channel->checkOperatorList(client)) {
        sendClientResponse(client, ":[IRC] 482 " + channel->getName() + " KICK :You're not channel operator\r\n");
        return;
    }
    
    std::string response = ":" + client->getNickName() + "!" + client->getNickName() + "@"+ client->getIpaddr()+ " KICK " + channel->getName() +  "\r\n";
    if (!message.empty()) {
        response += " :" + message;
    }
    response += "\r\n";
    sendClientResponse(target, response);
    channel->removeMember(target);
}