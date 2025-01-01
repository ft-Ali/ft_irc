#include "Bot.hpp"
#include "../../inc/Server.hpp"


int main() {
	Bot bot;
	bot.botInit();

}


/****
 * Bot doit prendre ip, port, pass, nick, user en argument
 * Msg de confirmation de connexion
 * Client = /bot, le bot doit pouvoir rejoindre le channel la ou le client a execute la commande /bot
 * Bot ouvre quotes.txt et envoie une quote aleatoire
 * channel #test : ficelo !quotes bot doit pouvoir sortir une quote dans le channel #test
*/


// refacto server.cpp
// handleClientMessage a refacto + handle privmsg
