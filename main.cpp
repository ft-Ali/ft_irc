// #include "inc/Server.hpp"
#include "inc/Channel.hpp"

int main (int c, char **v)
{
	if(c != 3) {
		std::cout << "Usage: " << v[0] << " <port> <password>" << std::endl;
		return 1;
	}
	else {
		Server server(atoi(v[1]), v[2]);
        signal(SIGINT, server.signalHandler);
        signal(SIGQUIT, server.signalHandler);
        // std::cout << "Server started mdp " << v[2] << std::endl;
        server.serverInit();
        server.serverLoop();

        
	}

	return 0;
}


// main https://www.geeksforgeeks.org/socket-programming-in-cpp/ pour comprendre des fonctionnes de base
/* int main()
{
    // creating socket
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    // specifying address
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    // sending connection request
    connect(clientSocket, (struct sockaddr*)&serverAddress,
            sizeof(serverAddress));

    // sending data
    const char* message = "Hello, server!";
    send(clientSocket, message, strlen(message), 0);

    // closing socket
    close(clientSocket);

    return 0;
} */

/****  

Étape 1 : Initialisation
Créer une socket pour le serveur.
Configurer les options de la socket (ex. réutilisation d'adresse).
Associer la socket au port donné.
Mettre la socket en mode écoute. 


*/