#include "../includes/server.hpp"

void Server::noSuchChannel(int fd, std::string channelName)
{
	Client *client = this->getClient(fd);
	std::string errMsg = ":ircserv 403 " + client->getFullIdenifer() + " " + channelName + " :No such channel";
	return sendMessage(fd, errMsg);
}

void Server::noSuchNick(int fd, std::string nickName)
{
	Client *client = this->getClient(fd);
	std::string errMsg = ":ircserv 401 " + client->getFullIdenifer() + " " + nickName + " :No such nick";
	return sendMessage(fd, errMsg);
}

void Server::permissionDenied(int fd, Channels channel)
{
	Client *client = this->getClient(fd);
	std::string errMsg = ":ircserv 482 " + client->getFullIdenifer() + " " + channel.getChannelName() + " :You are not channel operator!";
	return sendMessage(fd, errMsg);
}

void Server::notInThatChannel(int fd, Channels channel)
{
	Client *client = this->getClient(fd);
	std::string errMsg = ":ircserv 442 " + client->getFullIdenifer() + " " + channel.getChannelName() + " :You're not on that channel";
	return sendMessage(fd, errMsg);
}

void Server::invalidChannelName(int fd, std::string channelName)
{
	Client *client = this->getClient(fd);
	std::string errMsg = ":ircserv 476 " + client->getFullIdenifer() + " " + channelName + " :Invalid channel name, must start with #";
	return sendMessage(fd, errMsg);
}