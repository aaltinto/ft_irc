#include "../includes/server.hpp"
#include "../includes/client.hpp"
#include "../includes/channel.hpp"
#include <iostream>

void Server::addChannel(std::string channelName, Client client)
{
	Channels channel(client, channelName);
	this->_channels.push_back(channel);
	this->handleJoin(client, channel);
	std::cout << "Client " << client.getFd() << " joined " << channel.getChannelName() << std::endl;
}

// irc numeric replies
// https://gist.github.com/proxypoke/2264878
void Server::join(std::vector<std::string> args, int fd)
{
	if (args.size() <= 1)
		return;
	if (args[1][0] != '#')
	{
		std::cout << "Channel name has to start with '#'" << std::endl;
		return;
	}

	Client *client = this->getClient(fd);
	if (!client)
		throw std::runtime_error("Client not found");
	Channels *channel = this->getChannelbyName(args[1]);
	if (channel != NULL)
	{
		channel->joinChannel(*client);
		this->handleJoin(*client, *channel);
		client->addChannel(args[1]);
		return;
	}
	this->addChannel(args[1], *client);
}

void Server::nick(std::vector<std::string> args, int fd)
{
	if (args.size() < 2)
		throw std::runtime_error("Array out of bounds");
	int i = this->getClientIndex(fd);
	if (i == -1)
		throw std::runtime_error("Client not found");
	this->_clients[i].setNick(args[1]);
}

void Server::user(std::vector<std::string> args, int fd)
{
	if (args.size() < 5)
		throw std::runtime_error("Array out of bounds");

	int i = this->getClientIndex(fd);
	if (i == -1)
		throw std::runtime_error("Client not found");
	this->_clients[i].setUsername(args[1]);
	this->_clients[i].setRealName(args[4]);
}


void Server::privmsg(std::vector<std::string> args, int fd)
{
	if (args.size() < 3)
		throw std::runtime_error("Array out of bounds");
	Client *client = this->getClient(fd);
	if (!client)
		throw std::runtime_error("Client not found");
	std::string myMSG = ":" + client->getFullIdenifer() + " PRIVMSG " + args[1] + " :" + args[2];

	if (args[1][0] == '#')
	{
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
		throw std::runtime_error("Array out of bounds");
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
	Client *client = this->getClient(fd);
	std::string topicUpdate = ":" + client->getFullIdenifer() + " TOPIC " + channel->getChannelName() + " :" + args[2];
	channel->sendMessageToAll(topicUpdate);
}

void Server::part(std::vector<std::string> args, int fd)
{
	if (args.size() < 2)
		throw std::runtime_error("Array out of bounds");
	Channels *channel = this->getChannelbyName(args[1]);
	if (!channel)
		return this->noSuchChannel(fd, args[1]);
	Client *client = this->getClient(fd);
	if (!client)
		throw std::runtime_error("Client not found");
	channel->partChannel(*client);
	std::string partMessage;
	if (args.size() == 3)
		partMessage = ":" + client->getFullIdenifer() + " PART " + channel->getChannelName() + " :" + args[2];
	channel->sendMessageToAll(partMessage);
	client->removeChannel(args[1]);
}



void Server::quit(std::vector<std::string> args, int fd)
{
	if (args.size() < 2)
		throw std::runtime_error("Array out of bounds");
	this->clearClient(fd, args[1]);
}