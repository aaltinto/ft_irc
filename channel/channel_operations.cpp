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
	if (args.size() < 5)
	{
		std::cout << args.size();
		throw std::runtime_error("Array out of bounds");
	}
	int i = this->getClientIndex(fd);
	this->_clients[i].setUsername(args[1]);
	this->_clients[i].setRealName(args[4]);
	std::cout << this->_clients[i].getFullIdenifer() << std::endl;
}


void Server::privmsg(std::vector<std::string> args, int fd)
{
	std::cout << args.size() << std::endl;
	if (args.size() < 3)
		return;
	Client client = this->getClient(fd);
	std::string myMSG = ":" + client.getFullIdenifer() + " PRIVMSG " + args[1] + " :" + args[2];
	for (size_t i = 3; i < args.size(); i++)
	{
		myMSG += " ";
		myMSG += args[i];
	}
	std::cout << args[1];
	if (args[1][0] == '#')
	{
		std::cout << "Message sending all user in chanels" << std::endl;
		Channels *channel = this->getChannelbyName(args[1]);
		if (!channel)
			return this->noSuchChannel(fd, args[1]);
		return channel->sendMessageToAll(myMSG, fd);
	}
	int client_fd = this->getClientbyNick(args[1]);
	if (client_fd == -1)
		return this->noSuchNick(fd, args[1]);
	sendMessage(client_fd, myMSG);
}

void Server::topic(std::vector<std::string> args, int fd)
{
	if (args.size() < 2)
		return ;
	Channels *channel = this->getChannelbyName(args[1]);
	if (!channel)
		return this->noSuchChannel(fd, args[1]);
	if (args.size() == 2)
		return this->sendTopic(fd, *channel);
	if (!channel->isAdmin(fd))
	{
		std::cout << "Permission denied!" << std::endl;
		return this->permissionDenied(fd, *channel);
	}
	channel->setTopicName(args[2]);
	Client client = this->getClient(fd);
	std::string topicUpdate = ":" + client.getFullIdenifer() + " TOPIC " + channel->getChannelName() + " :" + args[2];
	channel->sendMessageToAll(topicUpdate);
}