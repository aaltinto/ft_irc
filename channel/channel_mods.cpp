#include "../includes/channel.hpp"
#include "../includes/mode.hpp"
#include "../includes/server.hpp"
#include <iostream>

void Channels::setMode(std::vector<Mode> modes, int fd)
{
    for (size_t i = 0; i < modes.size(); i++)
    {
        switch (modes[i].getFlag())
        {
            case 'i':
                this->inviteOnly(modes[i], fd);
                break;
            case 'o':
                this->setOperator(modes[i], fd);
                break;
            case 't':
                this->topicProtection(modes[i], fd);
                break;
            // case 'k':
            //     this->setKey(modes[i], fd);
            //     break;
            // case 'l':
            //     this->setLimit(modes[i], fd);
            //     break;
            default:
                this->unknownModeFlag(fd, modes[i].getFlag());
                break;
        }
    }
}

void Channels::inviteOnly(Mode mode, int fd)
{
    std::cout << "inviteOnly" << std::endl;
    std::string message;
    Client *client = this->getClient(fd);
    if (!client)
        return;
    if (mode.getSign() == 1)
    {
        if (this->_inviteOnly)
            return;
        this->_mods += "i";
        this->_inviteOnly = true;
        message = ":ircserv 324 " +  client->getFullIdenifer() + " " + this->getChannelName() + " +i";
    }
    else
    {
        if (!this->_inviteOnly)
            return;
        this->_mods.erase(this->_mods.find("i"), 1);
        message = ":ircserv 324 " +  client->getFullIdenifer() + " " + this->getChannelName() + " -i";
    }
    this->sendMessageToAll(message);
}

void Channels::setOperator(Mode mode, int fd)
{
    std::cout << "set operator" << std::endl;
    std::string message;
    Client *client = this->getClient(fd);
    if (!client)
        return;
    int clientToOpFd = this->getClientByNick(mode.getArg());
    if (clientToOpFd == -1)
    {
        std::cout << "No such nick" << std::endl;
        std::string errMsg = ":ircserv 401 " + client->getFullIdenifer() + " " + this->getChannelName() + " " + mode.getArg() + " :No such nick";
	    sendMessage(fd, errMsg);
        return ;
    }
    if (!this->checkClientIsIn(clientToOpFd))
    {
        std::cout << "Client is not in the channel" << std::endl;
        std::string errMsg = ":ircserv 441 " + client->getFullIdenifer() + " " + mode.getArg() + " " + this->getChannelName() + " :They aren't on that channel";
        sendMessage(fd, errMsg);
        return ;
    }
    Client *clientToOp = this->getClient(clientToOpFd);
    if (!clientToOp)
    {
        std::cerr << "No such client" << std::endl;
        return;
    }
    if (mode.getSign() == 1)
    {
        if (this->isAdmin(clientToOpFd))
            return;
        this->_mods += "o";
        this->_admins.push_back(*clientToOp);
        message = ":ircserv 324 " +  client->getFullIdenifer() + " " + this->getChannelName() + " +o " + mode.getArg();
    }
    else
    {
        if (this->isAdmin(clientToOpFd) == -1)
            return;
        this->_mods.erase(this->_mods.find("o"), 1);
        int i = this->getClientIndex(clientToOpFd);
        if (i == -1)
            return;
        this->_admins.erase(this->_admins.begin() + i);
        message = ":ircserv 324 " +  client->getFullIdenifer() + " " + this->getChannelName() + " -o" + mode.getArg();
    }
    this->sendMessageToAll(message);
}

void Channels::topicProtection(Mode mode, int fd)
{
    std::cout << "topicProtection" << std::endl;
    std::string message;
    Client *client = this->getClient(fd);
    if (!client)
        return;
    if (mode.getSign() == 1)
    {
        if (this->_topicProtection)
            return;
        this->_mods += "t";
        this->_topicProtection = true;
        message = ":ircserv 324 " +  client->getFullIdenifer() + " " + this->getChannelName() + " +t";
    }
    else
    {
        if (!this->_topicProtection)
            return;
        this->_mods.erase(this->_mods.find("t"), 1);
        this->_topicProtection = false;
        message = ":ircserv 324 " +  client->getFullIdenifer() + " " + this->getChannelName() + " -t";
    }
    this->sendMessageToAll(message);
}