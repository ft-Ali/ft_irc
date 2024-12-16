#include "inc/Server.hpp"
// #include "inc/Channel.hpp"

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

// void Server::handleNewConnection() {
//     int clientFd = accept(_serSocketFd, NULL, NULL); // accept new connection
//     if (clientFd == -1) 
//         throw(std::runtime_error("error: accept() failed"));
    
//     struct pollfd client;
//     client.fd = clientFd;
//     client.events = POLLIN;
//     fds.push_back(client);
//     std::string name;

//     if (_clientNicks.find(clientFd) == _clientNicks.end()) {
//         std::ostringstream oss;
//         int::iterator it = std::find(_clientNicks.begin(), _clientNicks.end(), clientFd);
//         if(it != _clientNicks.end()){
//             name = it->second;
//         }
//         oss << _clientNicks[clientFd] << clientFd;
//         _clientNicks[clientFd] = oss.str();
//     }
//     std::cout << "New client connected: FD " << clientFd << ", default nickname: " 
//               << _clientNicks[clientFd] << std::endl;
//     // Automatically send PASS command with the provided password
//     if(_password.empty()) {
//         std::string response = "Please provide a password using PASS <password>\n";
//         send(clientFd, response.c_str(), response.size(), 0);
//         return;
//     }
//     else {
//         handlePass(clientFd, "PASS " + _password, fds.size() - 1);
//     }

    
// }