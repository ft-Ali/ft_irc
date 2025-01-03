#include "Bot.hpp"
#include "../../inc/Server.hpp"


int main(int c, char **v) {
	if(c != 4) {
		std::cerr << "Usage: " << v[0] << " <ip> <port> <pass>" << std::endl;
		return 1;
	}
	Bot bot(atoi(v[2]), v[1], "FiceloBot", "FiceloBot", v[3]);
	bot.loadQuotes("srcs/Bot/quotes.txt");
	bot.botInit();
	bot.serverInit();
	return 0;

}

/****
 * Bot doit prendre ip, port, pass, nick, user en argument
 * Msg de confirmation de connexion
 * Client = /bot, le bot doit pouvoir rejoindre le channel la ou le client a execute la commande /bot
 * Bot ouvre quotes.txt et envoie une quote aleatoire
 * channel #test : ficelo !quotes bot doit pouvoir sortir une quote dans le channel #test
*/

