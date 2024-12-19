// #include "../../inc/Channel.hpp"
// #include "../../inc/Server.hpp"

// void    Server::removeClient(const std::string &command, Client *client){
//     std::vector<std::string> args = splitArg(command, ' ');

//     std::string channelName = args[1];
//     std::string message = "";

//     size_t msgIdx = command.find(':');
//     if(msgIdx != std::string::npos){
//         message = command.substr(msgIdx +1);
//     }

//     Channel *channel = getChannelByName(channelName);
//     if(!channel){
//         std::cout << channelName << "does not exist\n";
//         return ;
//     }

//     if(!channel->checkListMembers(client)){
//         std::cout <<  "You are not in "<<channelName << std::endl;
//         return ;
//     }
//     //send message
//         // channel->broadcastMessage(client, "PART " + channelName + " :" + message);

//     channel->leaveChannel(client);
// }
