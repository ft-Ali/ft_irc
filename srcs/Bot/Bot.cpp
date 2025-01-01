#include "Bot.hpp"

// void Bot::sendQuote() {
//     // ouvrir quotes.txt
//     // lire le fichier
//     // envoyer une quote aleatoire
//     // envoyer la quote sur le channel avec broadcastMessage
// }

void Bot::joinServer(int port, std::string ip, std::string pass)
{

   std::cout << "Connecting to server: " << ip << " on port " << port << " with password " << pass << std::endl;
}

void Bot::botInit() {
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
	addr.sin_port = htons(getPort());
	addr.sin_addr.s_addr = INADDR_ANY; //listening to any IP
    _serSocketFdBot = socket(AF_INET, SOCK_STREAM, 0);
	if (_serSocketFdBot == -1)
		throw(std::runtime_error("error: socket creation failed"));
    int en = 1;
	if (setsockopt(_serSocketFdBot, SOL_SOCKET, SO_REUSEADDR, &en, sizeof(en)) == -1)
		throw(std::runtime_error("error: failed to set option (O_NONBLOCK) on socket"));
	if (bind(_serSocketFdBot, (struct sockaddr *)&addr, sizeof(addr)) == -1)
		throw(std::runtime_error("error: failed to bind _serSocketFdBot to _port"));
	if (listen(_serSocketFdBot, SOMAXCONN) == -1)
		throw(std::runtime_error("listen() failed"));

    joinServer(Server::getInstance()->getPort(), Server::getInstance()->getIp(), Server::getInstance()->getPass());
}
