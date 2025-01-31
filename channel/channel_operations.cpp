#include "../includes/server.hpp"
#include "../includes/client.hpp"
#include "../includes/channel.hpp"
#include <iostream>

void Server::addChannel(std::string channelName, Client &client)
{
	Channels channel(client, channelName);
	this->_channels.push_back(channel);
	std::cout << "Client joined " << channel.getChannelName() << std::endl;
}

void Server::join(std::vector<std::string> args, int fd)
{
	std::string channelName;
	if (args.size() <= 1)
		return;
	if (args[1][0] != '#')
	{
		std::cout << "Channel name has to start with '#'" << std::endl;
		return;
	}
	channelName = args[1];

	int i;
	if (this->_channels.size() != 0)
	{
		i = this->getChannelIndex(channelName);
		if (i != -1)
		{
			this->_channels[i].joinChannel(this->getClient(fd));
			this->handleJoin(this->getClient(fd), this->_channels[i]);
			return;
		}
	}
	std::cout << "addChannel\n";
	this->addChannel(channelName, this->getClient(fd));
	this->handleJoin(this->getClient(fd), this->_channels[this->getChannelIndex(channelName)]);
	// irc numeric replies
	// https://gist.github.com/proxypoke/2264878

}

void Server::nick(std::vector<std::string> args, int fd)
{
	int i = this->getClientIndex(fd);
	this->_clients[i].setNick(args[1]);
}

void Server::user(std::vector<std::string> args, int fd)
{
	int i = this->getClientIndex(fd);
	this->_clients[i].setUsername(args[1]);
	this->_clients[i].setRealName(args[4]);
}