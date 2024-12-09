#include "inc/Server.hpp"

int main (int c, char **v)
{
	if(c != 3) {
		std::cout << "Usage: " << v[0] << " <port> <password>" << std::endl;
		return 1;
	}
	else {
		Server server(atoi(v[1]), v[2]);
		server.init();
	}


	return 0;
}

/****  

Étape 1 : Initialisation
Créer une socket pour le serveur.
Configurer les options de la socket (ex. réutilisation d'adresse).
Associer la socket au port donné.
Mettre la socket en mode écoute. 


*/