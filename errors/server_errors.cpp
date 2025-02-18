#include "../includes/server.hpp"

void noSuchChannel(Client *client, std::string channelName)
{
	if (!client)
		return;
	std::string errMsg = ":ircserv 403 " + client->getFullIdenifer() + " " + channelName + " :No such channel";
	return sendMessage(client->getFd(), errMsg);
}

#include <iostream>
void noSuchNick(Client *client, std::string nickName, std::string channelName)
{
	std::string errMsg;
	if (!client)
		return;
	if (channelName.empty())
	{
		std::string errMsg = ":ircserv 401 " + client->getFullIdenifer() + " " + nickName + " :No such nick";
		return sendMessage(client->getFd(), errMsg);
	}
	errMsg = ":ircserv 401 " + client->getFullIdenifer() + " " + channelName + " " + nickName + " :No such nick";
	sendMessage(client->getFd(), errMsg);
	errMsg = ":ircserv 401 " + client->getFullIdenifer() + " " + nickName + " :No such nick";
	return sendMessage(client->getFd(), errMsg);
}

void permissionDenied(Client *client, Channels channel)
{
	if (!client)
		return;
	std::string errMsg = ":ircserv 482 " + client->getFullIdenifer() + " " + channel.getChannelName() + " :You are not channel operator!";
	return sendMessage(client->getFd(), errMsg);
}

void notInThatChannel(Client *client, Channels channel)
{
	if (!client)
		return;
	std::string errMsg = ":ircserv 442 " + client->getFullIdenifer() + " " + channel.getChannelName() + " :You're not on that channel";
	return sendMessage(client->getFd(), errMsg);
}

void invalidChannelName(Client *client, std::string channelName)
{
	if (!client)
		return;
	std::string errMsg = ":ircserv 476 " + client->getFullIdenifer() + " " + channelName + " :Invalid channel name, must start with #";
	return sendMessage(client->getFd(), errMsg);
}

void notEnoughParameters(Client *client, std::string command)
{
	if (!client)
		return;
	std::string errMsg = ":ircserv 461 " + client->getFullIdenifer() + " " + command + " :Not enough parameters";
	return sendMessage(client->getFd(), errMsg);
}

void Channels::unknownModeFlag(int fd, char flag)
{
	Client *client = this->getClient(fd);
	if (!client)
		return;
	std::string errMsg = ":ircserv 501 " + client->getFullIdenifer() + " " + this->getChannelName() + " " + flag + " :is unknown mode flag";
	return sendMessage(fd, errMsg);
}