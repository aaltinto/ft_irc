#include "../includes/channel.hpp"
#include "../includes/mode.hpp"
#include "../includes/server.hpp"
#include <iostream>
#include <cstdlib>

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
            case 'k':
                this->setKey(modes[i], fd);
                break;
            case 'l':
                this->activateLimit(modes[i], fd);
                break;
            default:
                this->unknownModeFlag(fd, modes[i].getFlag());
                break;
        }
    }
}

void Channels::inviteOnly(Mode mode, int fd)
{
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
        this->_inviteOnly = false;
        if (this->_mods.find("i") == std::string::npos)
            return;
        this->_mods.erase(this->_mods.find("i"), 1);
        message = ":ircserv 324 " +  client->getFullIdenifer() + " " + this->getChannelName() + " -i";
    }
    this->sendMessageToAll(message);
}

void Channels::setOperator(Mode mode, int fd)
{
    std::string message;
    Client *client = this->getClient(fd);
    if (!client)
        return;
    int clientToOpFd = this->getClientByNick(mode.getArg());
    if (clientToOpFd == -1)
        return noSuchNick(client, mode.getArg(), this->getChannelName());
    if (!this->checkClientIsIn(clientToOpFd))
        return notInThatChannel(client, *this);
    Client *clientToOp = this->getClient(clientToOpFd);
    if (!clientToOp)
        return;
    if (mode.getSign() == 1)
    {
        if (this->isAdmin(clientToOpFd))
            return;
        std::string clientName = client->getFullIdenifer();
        this->adminOps(*clientToOp);
        
        message = ":ircserv 324 " + clientName  + " " + this->getChannelName() + " +o " + mode.getArg();
    }
    else
    {
        if (this->isAdmin(clientToOpFd) == -1)
            return;
        int i = this->getClientIndex(clientToOpFd);
        if (i == -1)
            return;
        this->adminOps(*clientToOp, false);
        message = ":ircserv 324 " +  client->getFullIdenifer() + " " + this->getChannelName() + " -o " + mode.getArg();
    }
    this->sendMessageToAll(message);
}

void Channels::topicProtection(Mode mode, int fd)
{
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
        if (this->_mods.find("t") == std::string::npos)
            return;
        this->_mods.erase(this->_mods.find("t"), 1);
        this->_topicProtection = false;
        message = ":ircserv 324 " +  client->getFullIdenifer() + " " + this->getChannelName() + " -t";
    }
    this->sendMessageToAll(message);
}

void Channels::setKey(Mode mode, int fd)
{
    Client *client = this->getClient(fd);
    std::string message;
    if (!client)
        return ;
    if (mode.getSign() == 1)
    {
        if (mode.getArg().empty())
            return notEnoughParameters(client, "MODE +k", this->getChannelName());
        if (this->isProtected() && this->getPass() == mode.getArg())
            return;
        if (!this->isProtected())
            this->_mods += mode.getFlag();
        this->setPass(mode.getArg());
        this->_IsProtected = true;
        message = ":ircserv 324 " +  client->getFullIdenifer() + " " + this->getChannelName() + " +k " + mode.getArg();
    }
    else
    {
        if (!this->isProtected())
            return ;
        this->_IsProtected = false;
        this->_mods.erase(this->_mods.find("k"), 1);
        this->setPass("");
        message = ":ircserv 324 " +  client->getFullIdenifer() + " " + this->getChannelName() + " -k";
    }
    this->sendMessageToAll(message);
}

void Channels::activateLimit(Mode mode, int fd)
{
    Client *client = this->getClient(fd);
    std::string message;
    if (!client)
    return ;
    if (mode.getSign() == 1)
    {
        if (mode.getArg().empty())
            return notEnoughParameters(client, "MODE +l", this->getChannelName());
        for (size_t i = 0; i < mode.getArg().size(); i++)
            if (!std::isdigit(mode.getArg()[i]))
                return invalidArgument(client,
                    std::string(1, mode.getSign() ? '+' : '-') + std::string(1, mode.getFlag()),
                    mode.getArg(), this->getChannelName(), "Positive integer expected");
        int num = std::atoi(mode.getArg().c_str());
        if (num <= 0)
        {
            mode.setSign(-1);
            this->activateLimit(mode, fd);
            return;
        }

        if (_limit == -1)
            this->_mods += "l";
        this->setLimit(num);
        message = ":ircserv 324 " + client->getFullIdenifer() + " " + this->getChannelName() + " +l " + mode.getArg();
    }
    else
    {
        if (_limit != -1)
        {

            this->_mods.erase(this->_mods.find("l"), 1);
        }
        this->setLimit();
        message = ":ircserv 324 " + client->getFullIdenifer() + " " + this->getChannelName() + " -l";
    }
    this->sendMessageToAll(message);
}
