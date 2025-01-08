#include "../../inc/Channel.hpp"
#include "../../inc/Server.hpp"

void Server::handleInvit(Client *client, std::string command) {
    std::vector<std::string> cmd = splitArg(command, ' ');
    if (cmd.size() < 3) {
        sendClientResponse(client, ":[IRC] 461 " + client->getNickName() + " INVITE :Not enough parameters\r\n");
        return;
    }

    std::string targetName = cmd[1];
    std::string targetChannel = cmd[2];

    Channel *channel = getChannelByName(targetChannel);
    if (!channel) {
        sendClientResponse(client, ":[IRC] 403 " + targetChannel + " :No such channel\r\n");
        return;
    }

    Client *target = getClientByName(targetName);
    if (!target) {
        sendClientResponse(client, ":[IRC] 401 " + targetName + " :No such\r\n");
        return;
    }

    if (!channel->checkOperatorList(client)) {
        sendClientResponse(client, ":[IRC] 482 " + targetChannel + " :You're not channel operator\r\n");
        return;
    }

    if (channel->checkListMembers(target)) {
        sendClientResponse(client, ":[IRC] 443 " + targetName + " " + targetChannel + " :is already on channel\r\n");
        return;
    }

    channel->addToWhiteList(target);
    std::string response = ":" + client->getNickName() + "!" + client->getHost() + " INVITE " + target->getNickName() + " :" + channel->getName() + "\r\n";
    sendClientResponse(target, response);
}
