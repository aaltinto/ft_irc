#include "../includes/server.hpp"
#include <iostream>

void noSuchChannel(Client *client, std::string channelName)
{
	if (!client)
		return;
	std::string errMsg = ":ircserv 403 " + client->getFullIdenifer() + " " + channelName + " :No such channel";
	return sendMessage(client->getFd(), errMsg);
}

void noSuchNick(Client *client, std::string nickName, std::string channelName)
{
	std::string errMsg;
	if (!client)
		return;
	if (channelName.empty())
	{
		errMsg = ":ircserv 401 " + client->getFullIdenifer() + " " + nickName + " :No such nick";
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

void notEnoughParameters(Client *client, std::string command, std::string channelName)
{
	std::string errMsg;
	if (!client)
		return;
	if (channelName.empty())
	{
		errMsg = ":ircserv 461 " + client->getFullIdenifer() + " " + command + " :Not enough parameters";
		return sendMessage(client->getFd(), errMsg);
	}
	errMsg = ":ircserv 461 " + client->getFullIdenifer() + " " + channelName + " " + command + " :Not enough parameters";
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

void inviteOnlyChannel(Client *client, Channels *channel)
{
	std::cout << "Channel is invite only!" << std::endl;
	if (!client || !channel)
		return;
	std::string errMsg = ":server 473 "
						+ client->getFullIdenifer() + " "
						+ channel->getChannelName()
						+ " :Cannot join channel (+i)";
	return sendMessage(client->getFd(), errMsg);
}

void passwordRequired(Client *client, Channels *channel)
{
	if (!client)
		return;
	std::string channelName = "";
	if (channel)
		channelName = channel->getChannelName();
	std::string errMsg = ":server 461 " + client->getFullIdenifer() + " " + channelName + " :Password required";
	return sendMessage(client->getFd(), errMsg);
}

void passwordIncorrect(Client *client, Channels *channel)
{
	if (!client)
		return;
	std::string channelName = "";
	if (channel)
		channelName = channel->getChannelName();
	std::string errMsg = ":server 464 " + client->getFullIdenifer() + " " + channelName + " :Password incorrect";
	return sendMessage(client->getFd(), errMsg);
}

void channelLimitExceeded(Client *client, Channels *channel)
{
	if (!client || !channel)
		return;
	std::string errMsg = ":server 471 " + client->getFullIdenifer() + " " + channel->getChannelName() + " :Cannot join channel (+l)";
	return sendMessage(client->getFd(), errMsg);
}

void nickInUse(Client *client, std::string nick)
{
	if (!client)
		return;
	std::string errMsg = ":server 433 " + client->getFullIdenifer() + " " + nick + " :Nickname is already in use";
	return sendMessage(client->getFd(), errMsg);
}
