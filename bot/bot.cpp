#include "bot.hpp"
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <sstream>
#include <cerrno>  // errno için eklenmeli

// Command handler implementations
void HelpCommandHandler::execute(Bot* bot, const std::string& sender, const std::string& target, const std::vector<std::string>& args) {
    bot->cmdHelp(sender, target, args);
}

void PingCommandHandler::execute(Bot* bot, const std::string& sender, const std::string& target, const std::vector<std::string>& args) {
    bot->cmdPing(sender, target, args);
}

void InfoCommandHandler::execute(Bot* bot, const std::string& sender, const std::string& target, const std::vector<std::string>& args) {
    bot->cmdInfo(sender, target, args);
}

Bot::Bot(const std::string& server, int port, const std::string& password, const std::string& nickname)
    : _socket(-1), _server(server), _port(port), _password(password), 
      _nickname(nickname), _connected(false) {
    
    _username = nickname;
    _realname = "IRC Bot";
    registerCommands();
}

Bot::~Bot() {
    if (_socket != -1) {
        sendRaw("QUIT :Bot shutting down");
        close(_socket);
    }
    
    // Clean up command handlers
    std::map<std::string, CommandHandler*>::iterator it;
    for (it = _commands.begin(); it != _commands.end(); ++it) {
        delete it->second;
    }
    _commands.clear();
}

void Bot::registerCommands() {
    _commands["help"] = new HelpCommandHandler();
    _commands["ping"] = new PingCommandHandler();
    _commands["info"] = new InfoCommandHandler();
}

bool Bot::connect() {
    struct sockaddr_in serverAddr;
    
    // Create socket
    _socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_socket == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        return false;
    }
    
    // Prepare server address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(_port);
    
    if (inet_pton(AF_INET, _server.c_str(), &serverAddr.sin_addr) <= 0) {
        std::cerr << "Invalid address/ Address not supported" << std::endl;
        return false;
    }
    
    // Connect to server
    if (::connect(_socket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Connection Failed: " << strerror(errno) << std::endl;
        return false;
    }
    
    std::cout << "Connected to " << _server << ":" << _port << std::endl;
    return true;
}

bool Bot::authenticate() {
    // Send password
    if (!_password.empty() && !sendRaw("PASS " + _password)) {
        return false;
    }
    
    // Send nickname
    if (!sendRaw("NICK " + _nickname)) {
        return false;
    }
    
    // Send user information
    if (!sendRaw("USER " + _username + " 0 * :" + _realname)) {
        return false;
    }
    
    std::cout << "Authentication sent" << std::endl;
    return true;
}

bool Bot::start() {
    if (!connect()) {
        return false;
    }
    
    if (!authenticate()) {
        close(_socket);
        return false;
    }
    
    _connected = true;
    return true;
}

bool Bot::joinChannel(const std::string& channel) {
    if (!_connected) {
        return false;
    }
    
    std::string channelName = channel;
    if (channel[0] != '#') {
        channelName = "#" + channel;
    }
    
    if (sendRaw("JOIN " + channelName)) {
        _channels.push_back(channelName);
        std::cout << "Joined channel: " << channelName << std::endl;
        return true;
    }
    
    return false;
}

bool Bot::sendRaw(const std::string& message) {
    std::string fullMessage = message + "\r\n";
    if (send(_socket, fullMessage.c_str(), fullMessage.length(), 0) < 0) {
        std::cerr << "Failed to send message: " << strerror(errno) << std::endl;
        return false;
    }
    return true;
}

bool Bot::sendMessage(const std::string& target, const std::string& message) {
    return sendRaw("PRIVMSG " + target + " :" + message);
}

void Bot::parseCommand(const std::string& sender, const std::string& target, const std::string& message) {
    std::istringstream iss(message.substr(1)); // Remove the ! prefix
    std::string command;
    iss >> command;
    
    std::vector<std::string> args;
    std::string arg;
    while (iss >> arg) {
        args.push_back(arg);
    }
    
    // Check if command exists in our commands map
    std::map<std::string, CommandHandler*>::iterator it = _commands.find(command);
    if (it != _commands.end()) {
        it->second->execute(this, sender, target, args);
    }
}

// Modified command methods to use the target parameter
void Bot::cmdHelp(const std::string& sender, const std::string& target, const std::vector<std::string>& /* args */) {
    std::string helpMsg = "Available commands: !help, !ping, !info";
    sendMessage(target, helpMsg);
}   

void Bot::cmdPing(const std::string& sender, const std::string& target, const std::vector<std::string>& /* args */) {
    sendMessage(target, "Pong!");
}

void Bot::cmdInfo(const std::string& sender, const std::string& target, const std::vector<std::string>& /* args */) {
    std::stringstream ss;
    ss << _port;
    std::string infoMsg = "I'm a C++ IRC bot. Connected to: " + _server + ":" + ss.str();
    infoMsg += " | Channels: ";
    for (size_t i = 0; i < _channels.size(); ++i) {
        infoMsg += _channels[i];
        if (i < _channels.size() - 1) {
            infoMsg += ", ";
        }
    }
    sendMessage(target, infoMsg);
}

void Bot::processMessage(const std::string& message) {
    std::cout << "Processing: " << message << std::endl;
    
    // PING mesajlarını yanıtla
    if (message.find("PING") == 0) {
        std::string pong = "PONG" + message.substr(4);
        sendRaw(pong);
        return;
    }
    
    // JOIN doğrulamasını kontrol et
    size_t pos = message.find(" JOIN ");
    if (pos != std::string::npos) {
        std::cout << "Channel join confirmed: " << message << std::endl;
        return;
    }
    
    // Özel mesajları işle
    if (message.find("PRIVMSG") != std::string::npos) {
        // Gönderici bilgisini çıkar
        size_t nickEnd = message.find('!');
        if (nickEnd != std::string::npos) {
            std::string sender = message.substr(1, nickEnd - 1);
            
            // Hedef (kanal veya kullanıcı) bilgisini çıkar
            size_t targetStart = message.find("PRIVMSG ") + 8;
            size_t targetEnd = message.find(" :", targetStart);
            
            if (targetEnd != std::string::npos) {
                std::string target = message.substr(targetStart, targetEnd - targetStart);
                std::string content = message.substr(targetEnd + 2);
                
                // Komut olup olmadığını kontrol et
                if (content[0] == '!') {
                    // Hedef botun kendi ismi ise (özel mesaj), yanıtı gönderene yolla
                    std::string responseTarget;
                    if (target == _nickname) {
                        responseTarget = sender; // Özel mesaj durumunda gönderene yanıt ver
                    } else {
                        responseTarget = target; // Kanal mesajı durumunda kanala yanıt ver
                    }
                    parseCommand(sender, responseTarget, content);
                }
            }
        }
    }
    
    // IRC protokolünün diğer mesajlarını işle
    // ...
}

// Terminalde kullanılabilecek komutları ekle
void Bot::handleTerminalCommand(const std::string& cmd) {
    std::istringstream iss(cmd);
    std::string command;
    iss >> command;
    
    if (command == "join" || command == "JOIN") {
        std::string channel;
        if (iss >> channel) {
            joinChannel(channel);
        } else {
            std::cout << "Usage: join #channel" << std::endl;
        }
    }
    else if (command == "msg" || command == "PRIVMSG") {
        std::string target, message;
        if (iss >> target) {
            std::getline(iss, message);
            if (!message.empty()) {
                // İlk boşluğu kaldır
                if (message[0] == ' ')
                    message = message.substr(1);
                sendMessage(target, message);
            } else {
                std::cout << "Usage: msg <target> <message>" << std::endl;
            }
        }
    }
    else if (command == "raw") {
        std::string rawCmd;
        std::getline(iss, rawCmd);
        if (!rawCmd.empty()) {
            // İlk boşluğu kaldır
            if (rawCmd[0] == ' ')
                rawCmd = rawCmd.substr(1);
            sendRaw(rawCmd);
        } else {
            std::cout << "Usage: raw <irc_command>" << std::endl;
        }
    }
    else if (command == "quit" || command == "exit") {
        std::cout << "Disconnecting..." << std::endl;
        _connected = false;
    }
    else if (command == "help") {
        std::cout << "Available commands:" << std::endl;
        std::cout << "  join #channel     - Join a channel" << std::endl;
        std::cout << "  msg target text   - Send a private message" << std::endl;
        std::cout << "  raw command       - Send a raw IRC command" << std::endl;
        std::cout << "  quit/exit         - Disconnect and exit" << std::endl;
        std::cout << "  help              - Show this help" << std::endl;
    }
    else {
        std::cout << "Unknown command. Type 'help' for available commands." << std::endl;
    }
}

void Bot::runLoop() {
    char buffer[4096];
    int bytesReceived;
    fd_set readfds;
    struct timeval tv;
    int max_fd = _socket;
    
    std::cout << "Bot is running. Type 'help' for available commands." << std::endl;
    
    while (_connected) {
        // Dosya tanımlayıcı kümesini hazırla
        FD_ZERO(&readfds);
        FD_SET(_socket, &readfds);
        FD_SET(STDIN_FILENO, &readfds); // Klavye girişini de dinle
        
        // Select için timeout ayarı
        tv.tv_sec = 0;
        tv.tv_usec = 100000; // 100ms
        
        // Select ile hem soket hem de terminal girişini dinle
        int activity = select(max_fd + 1, &readfds, NULL, NULL, &tv);
        
        if (activity < 0) {
            std::cerr << "Select error" << std::endl;
            break;
        }
        
        // Soket mesajlarını kontrol et
        if (FD_ISSET(_socket, &readfds)) {
            memset(buffer, 0, sizeof(buffer));
            bytesReceived = recv(_socket, buffer, sizeof(buffer) - 1, 0);
            
            if (bytesReceived > 0) {
                buffer[bytesReceived] = '\0';
                std::string data(buffer);
                
                // Alınan verileri satırlara ayır
                std::istringstream stream(data);
                std::string line;
                
                while (std::getline(stream, line)) {
                    // CR karakterini temizle
                    if (!line.empty() && line[line.length()-1] == '\r') {
                        line.erase(line.length()-1);
                    }
                    
                    if (!line.empty()) {
                        std::cout << "< " << line << std::endl; // Gelen veriyi log kayıtla
                        processMessage(line);
                    }
                }
            }
            else if (bytesReceived <= 0) {
                _connected = false;
                std::cerr << "Connection closed or error" << std::endl;
            }
        }
        
        // Terminal girişini kontrol et
        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            std::string cmd;
            std::getline(std::cin, cmd);
            
            if (!cmd.empty()) {
                handleTerminalCommand(cmd);
            }
        }
    }
    
    std::cout << "Bot has disconnected." << std::endl;
}
