#include "Bot.hpp"
#include "../../inc/Server.hpp"

// Variable globale pour pointer vers l'instance du bot
Bot* globalBot = NULL;
volatile bool shouldClose = false;

void Bot::closeConnection() {
    if (_serSocketBot != -1) {
        close(_serSocketBot);
        _serSocketBot = -1;
    }
    std::vector<std::string>().swap(_quotes);
}

void signalHandler(int signal) {
    (void)signal;
    if (globalBot) {
        std::cout << "Signal received. Cleaning up bot." << std::endl;
        globalBot->closeConnection();  // Libérer les ressources du bot
        shouldClose = true;  // Indiquer qu'il faut fermer le programme proprement
    }
}

int main(int argc, char** argv) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <ip> <port> <pass>" << std::endl;
        return 1;
    }

    try {
        Bot bot(atoi(argv[2]), argv[1], "FiceloBot", "FiceloBot", argv[3]);
        globalBot = &bot; // Assignation de l'instance du bot à la variable globale

        // Définir le gestionnaire de signaux
        signal(SIGINT, signalHandler);  // SIGINT (Ctrl+C)
        signal(SIGQUIT, signalHandler); // SIGQUIT

        // Charger les citations et démarrer le bot
        bot.loadQuotes("srcs/Bot/quotes.txt");
        bot.botInit();
        bot.listenToServer();

        // Nettoyage final
        if (shouldClose) {
            std::cout << "Shutting down bot gracefully." << std::endl;
        }

        return 0;
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}

/****
 * Bot doit prendre ip, port, pass, nick, user en argument
 * Msg de confirmation de connexion
 * Client = /bot, le bot doit pouvoir rejoindre le channel la ou le client a execute la commande /bot
 * Bot ouvre quotes.txt et envoie une quote aleatoire
 * channel #test : ficelo !quotes bot doit pouvoir sortir une quote dans le channel #test
*/

