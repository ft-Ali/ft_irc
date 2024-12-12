// #include "inc/Server.hpp"
#include "inc/Channel.hpp"

int main (int c, char **v)
{
	(void)c;
	(void)v;
    Client client("eugene");
	

	try{
        Channel channel(&client ,"#BOnj-gf");	}
	catch (std::exception &e){
		std::cerr << e.what();
        // exit(0);
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