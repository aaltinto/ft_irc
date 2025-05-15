#pragma once

#include <string>
#include <vector>
#include <map>

class Bot;

// Command handler class for C++98 compatibility
class CommandHandler {
public:
    virtual ~CommandHandler() {}
    virtual void execute(Bot* bot, const std::string& sender, const std::string& target, const std::vector<std::string>& args) = 0;
};

// Specific command handlers
class HelpCommandHandler : public CommandHandler {
public:
    void execute(Bot* bot, const std::string& sender, const std::string& target, const std::vector<std::string>& args);
};

class PingCommandHandler : public CommandHandler {
public:
    void execute(Bot* bot, const std::string& sender, const std::string& target, const std::vector<std::string>& args);
};

class InfoCommandHandler : public CommandHandler {
public:
    void execute(Bot* bot, const std::string& sender, const std::string& target, const std::vector<std::string>& args);
};

class Bot {
public:
    Bot(const std::string& server, int port, const std::string& password, const std::string& nickname);
    ~Bot();
    
    bool start();
    bool joinChannel(const std::string& channel);
    bool sendMessage(const std::string& target, const std::string& message);
    void runLoop();
    
    // Bot commands - now made public so handler classes can access them
    void cmdHelp(const std::string& sender, const std::string& target, const std::vector<std::string>& args);
    void cmdPing(const std::string& sender, const std::string& target, const std::vector<std::string>& args);
    void cmdInfo(const std::string& sender, const std::string& target, const std::vector<std::string>& args);
    
    void handleTerminalCommand(const std::string& cmd);

private:
    int _socket;
    std::string _server;
    int _port;
    std::string _password;
    std::string _nickname;
    std::string _username;
    std::string _realname;
    bool _connected;
    std::vector<std::string> _channels;
    std::map<std::string, CommandHandler*> _commands;
    
    // Helper methods
    bool connect();
    bool authenticate();
    bool sendRaw(const std::string& message);
    void processMessage(const std::string& message);
    void parseCommand(const std::string& sender, const std::string& target, const std::string& message);
    void registerCommands();
};
