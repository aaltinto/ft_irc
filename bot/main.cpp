#include "bot.hpp"
#include <iostream>
#include <string>
#include <csignal>
#include <cstdlib>  // For exit() and atoi()

Bot* gBot = NULL;

void signalHandler(int signum) {
    std::cout << "Received signal: " << signum << std::endl;
    if (gBot) {
        delete gBot;
        gBot = NULL;
    }
    exit(signum);  // Now exit is properly declared
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cerr << "Usage: " << argv[0] << " <server_address> <port> <password> [nickname] [channels]" << std::endl;
        std::cerr << "Example: " << argv[0] << " 127.0.0.1 6667 password MyBot #channel1,#channel2" << std::endl;
        return 1;
    }
    
    std::string server = argv[1];
    int port = atoi(argv[2]);  // Use atoi directly without std:: namespace
    std::string password = argv[3];
    std::string nickname = "IRCBot";
    
    if (argc > 4) {
        nickname = argv[4];
    }
    
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    
    // Create and start the bot
    gBot = new Bot(server, port, password, nickname);
    
    if (!gBot->start()) {
        std::cerr << "Failed to start the bot" << std::endl;
        delete gBot;
        return 1;
    }
    
    // Join channels if specified
    if (argc > 5) {
        std::string channelList = argv[5];
        size_t pos = 0;
        std::string channel;
        
        while ((pos = channelList.find(',')) != std::string::npos) {
            channel = channelList.substr(0, pos);
            gBot->joinChannel(channel);
            channelList.erase(0, pos + 1);
        }
        
        if (!channelList.empty()) {
            gBot->joinChannel(channelList);
        }
    }
    
    // Run the bot's main loop
    gBot->runLoop();
    
    delete gBot;
    return 0;
}
