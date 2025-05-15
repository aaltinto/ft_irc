#ifndef BOT_H
#define BOT_H

#include <string>

class Bot {
public:
    Bot(const std::string& host, int port);
    void connect();
    void disconnect();
    void sendMessage(const std::string& message);
    void receiveMessage();
    void runLoop();

private:
    std::string host;
    int port;
    int socketFd;
    bool isConnected; // Bağlantı durumunu takip etmek için
};

#endif // BOT_H