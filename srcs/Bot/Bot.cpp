#include "Bot.hpp"

// ====================== UTILS ======================
// ###################################################
// #                UTILS FUNCTIONS                #
// ###################################################

// function to send a message to a channel
void Bot::sendMessageToChannel(const std::string &target, const std::string &message) {
    sendCommand("PRIVMSG " + target + " :" + message + "\r\n");
}

// function to send a command to the server
void Bot::sendCommand(const std::string &command) {
    if (send(_serSocketBot, command.c_str(), command.size(), 0) == -1) {
        throw std::runtime_error("Failed to send command: " + command);
    }
}

// function to load quotes from a file
void Bot::loadQuotes(const std::string &filePath) {
    std::ifstream file(filePath.c_str());
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file at path: " << filePath << std::endl;
        throw std::runtime_error("Failed to open quotes file: " + filePath);
    }
    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty()) {
            _quotes.push_back(line);
        }
    }
    file.close();
    if (_quotes.empty()) {
        throw std::runtime_error("No quotes found in file: " + filePath);
    }
    std::cout << "Loaded " << _quotes.size() << " quotes from " << filePath << std::endl;
}

// function to send a random quote to a channel
void Bot::sendRandomQuote(const std::string &channel) {
    if (_quotes.empty()) {
        std::cerr << "No quotes loaded to send." << std::endl;
        return;
    }
    std::srand(std::time(0));
    int randomIndex = std::rand() % _quotes.size();
    std::string message = _quotes[randomIndex];

    sendMessageToChannel(channel, message);
}

// ====================== IMPORTANT FUNCTIONS ======================
// ###################################################
// #          IMPORTANT BOT LOGIC FUNCTIONS         #
// ###################################################


// function to handle a command received from the server
void Bot::handleCommand(const std::string &message) {
    size_t privmsgPos = message.find("PRIVMSG");
    if (privmsgPos != std::string::npos) {
        size_t channelStart = message.find(' ', privmsgPos) + 1;
        size_t channelEnd = message.find(' ', channelStart);
        std::string channel = message.substr(channelStart, channelEnd - channelStart);

        size_t messageStart = message.find(":", channelEnd) + 1;
        std::string msgContent = message.substr(messageStart);
        if (msgContent.find("!quotes") == 0) {
            sendRandomQuote(channel);
        }
    }
}

// function to listen to the server for incoming messages
void Bot::listenToServer() {
    char buffer[1024];

    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(_serSocketBot, buffer, sizeof(buffer) - 1, 0);

        if (bytesReceived <= 0) {
            std::cerr << "Connection closed by server or error occurred." << std::endl;
            break;
        }

        std::string message(buffer);
        if (message.find("PING") == 0) {
            std::string pongResponse = "PONG " + message.substr(5) + "\r\n";
            sendCommand(pongResponse);
            continue;
        }
        handleCommand(message);
    }
}


// function to join the server
void Bot::joinServer() {
    struct sockaddr_in serverAddr;
    struct pollfd communication;

    // Si une socket est déjà ouverte, la fermer proprement avant de tenter de se reconnecter
    if (_serSocketBot != -1) {
        close(_serSocketBot);  // Ferme la socket existante si elle est encore ouverte
        std::cout << "Socket closed before reconnecting." << std::endl;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(_port);

    // Crée une nouvelle socket
    _serSocketBot = socket(AF_INET, SOCK_STREAM, 0);
    if (_serSocketBot == -1) {
        throw std::runtime_error("Socket creation failed");
    }
    // _serSocketBot += 1;
    if (inet_pton(AF_INET, _ip.c_str(), &serverAddr.sin_addr) <= 0) {
        throw std::runtime_error("Invalid IP address: " + _ip);
    }

    // Tentative de connexion
    if (connect(_serSocketBot, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Connection error");
        throw std::runtime_error("Connection to server failed");
    }
    std::cout << "Connected to server: " << _ip << " on port " << _port << std::endl;

    // Ajouter le descripteur de socket à la liste des descripteurs surveillés
    communication.fd = _serSocketBot;
    communication.events = POLLIN;
    communication.revents = 0;
    fds.push_back(communication);

    // Envoie des commandes initiales après la connexion
    sendCommand("PASS " + _password + "\r\n");
    sendCommand("NICK " + _nick + "\r\n");
    sendCommand("USER " + _user + " 0 * :" + _user + "\r\n");
}

// function to initialize the bot
void Bot::botInit() {
    try {
    std::cout << "Bot initialized with NICK: " << _nick << " and USER: " << _user << std::endl;
    joinServer();
    std::string channel = "#IA";
    sendCommand("JOIN " + channel + "\r\n");
    std::cout << "Bot has joined channel " << channel << std::endl;
    std::string welcomeMessage = "Hello, I'm FiceloBot, and I'm here to help!";
    sendMessageToChannel(channel, welcomeMessage);
    listenToServer();
    }
    catch(std::exception &e){
        std::cout << e.what() << std::endl;
    }
}
