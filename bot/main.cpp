#include "bot.hpp"
#include <signal.h>
#include <vector>

std::vector<std::string> splitChannels(const std::string& channelsStr) {
    std::vector<std::string> channels;
    size_t start = 0, end;
    while ((end = channelsStr.find(',', start)) != std::string::npos) {
        channels.push_back(channelsStr.substr(start, end - start));
        start = end + 1;
    }
    channels.push_back(channelsStr.substr(start));

    for (size_t i = 0; i < channels.size(); ++i) {
        if (channels[i][0] != '#') {
            channels[i] = '#' + channels[i];
        }
    }
    for (size_t i = 0; i < channels.size(); ++i) 
    {
        std::cout << "Channel " << i + 1 << ": " << channels[i] << std::endl;
    }
    return channels;
}

int main(int argc, char* argv[]) {
    if (argc < 6) {
        std::cerr << "Usage: " << argv[0] << " <server> <port> <password> <nickname> \"[#channel,#channel...]\"" << std::endl;
        return 1;
    }

    std::string server = argv[1];
    std::string port = argv[2];
    std::string password = argv[3];
    std::string nickname = argv[4];
    
    bot myBot(server, port, password, nickname, splitChannels(argv[5]));
    myBot.run();
    myBot.disconnect();

    return 0;
}