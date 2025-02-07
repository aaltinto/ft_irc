#include "../includes/server.hpp"
#include "../includes/client.hpp"
#include "../includes/channel.hpp"
#include <iostream>

void Server::addChannel(std::string channelName, Client client)
{
	if (client.isAuth() == false)
		return;
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

	std::cout << "join" << std::endl;
	Client *client = this->getClient(fd);
	if (!client)
	{
		std::cout << "Client not found!" << std::endl;
		return;
	}
	if (client->isAuth() == false)
	{
		std::cout << "Client not authed!" << std::endl;
		return;
	}
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

	std::cout << "nick" << std::endl;
	if (args.size() < 2)
		throw std::runtime_error("Array out of bounds");
	int i = this->getClientIndex(fd);
	if (i == -1)
		return;
	if (this->_clients[i].isAuth() == false)
	{
		std::cout << "Password required!" << std::endl;
		std::string msg = ":server 461 " + this->_clients[i].getFullIdenifer() + " PASS :Not enough paramaters!";
		sendMessage(fd, msg);
		this->clearClient(fd);
		return;
	}
	this->_clients[i].setNick(args[1]);
}

void Server::user(std::vector<std::string> args, int fd)
{
	std::cout << "user" << std::endl;
	if (args.size() < 5)
		throw std::runtime_error("Array out of bounds");

	int i = this->getClientIndex(fd);
	if (i == -1)
		return;
	if (this->_clients[i].isAuth() == false)
		return;
	this->_clients[i].setUsername(args[1]);
	this->_clients[i].setRealName(args[4]);
}


void Server::privmsg(std::vector<std::string> args, int fd)
{
	std::cout << "messgae" << std::endl;
	if (args.size() < 3)
		throw std::runtime_error("Array out of bounds");
	Client *client = this->getClient(fd);
	if (!client)
		return;
	if (client->isAuth() == false)
	{
		std::cout << "client is not auth!? \n";
		return;
	}
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
	std::cout << "topic" << std::endl;
	if (args.size() < 2)
		throw std::runtime_error("Array out of bounds");
	Client *client = this->getClient(fd);
	if (!client)
		return;
	if (client->isAuth() == false)
		return;
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
	std::string topicUpdate = ":" + client->getFullIdenifer() + " TOPIC " + channel->getChannelName() + " :" + args[2];
	channel->sendMessageToAll(topicUpdate);
}

void Server::part(std::vector<std::string> args, int fd)
{
	std::cout << "part" << std::endl;
	if (args.size() < 2)
		throw std::runtime_error("Array out of bounds");
	Client *client = this->getClient(fd);
	if (!client)
		return;
	if (client->isAuth() == false)
		return;
	Channels *channel = this->getChannelbyName(args[1]);
	if (!channel)
		return this->noSuchChannel(fd, args[1]);
	std::string partMessage;
	if (args.size() == 3)
		partMessage = ":" + client->getFullIdenifer() + " PART " + channel->getChannelName() + " :" + args[2];
	channel->sendMessageToAll(partMessage);
	client->removeChannel(args[1]);
	channel->partChannel(*client);
}



void Server::quit(std::vector<std::string> args, int fd)
{
	std::cout << "quit" << std::endl;
	if (args.size() < 2)
		throw std::runtime_error("Array out of bounds");
	int i = this->getClientIndex(fd);
	if (i == -1)
		return;
	this->clearClient(fd, args[1]);
}

void Server::kick(std::vector<std::string> args, int fd)
{
	std::cout << "kick" << std::endl;
	if (args.size() < 3)
		throw std::runtime_error("Array out of bounds");
	
	Client *kicker = this->getClient(fd);
	if (!kicker)
		return;
	if (kicker->isAuth() == false)
		return;
	Channels *channel = this->getChannelbyName(args[1]);
	if (!channel)
		return this->noSuchChannel(fd, args[1]);
	if (!channel->isAdmin(fd))
	{
		std::cout << "Permission denied!" << std::endl;
		return this->permissionDenied(fd, *channel);
	}
	int client_fd = channel->getClientByNick(args[2]);
	if (client_fd == -1)
		return this->noSuchNick(fd, args[2]);
	Client *client = this->getClient(client_fd);
	std::string kickMsg = ":" + kicker->getFullIdenifer() +
						" KICK " + channel->getChannelName() +
						" " + args[2];
	if (args.size() == 4)
		kickMsg += " :" + args[3];

	channel->sendMessageToAll(kickMsg);
	client->removeChannel(args[1]);
	channel->partChannel(*client);
}

void Server::pass(std::vector<std::string> args, int fd)
{
	std::cout << "pass" << std::endl;
	int i = this->getClientIndex(fd);
	if (i == -1)
		return;
	if (args.size() < 2)
	{
		std::cout << "Password required!" << std::endl;
		std::string msg = ":server 461 " + this->_clients[i].getFullIdenifer() + " PASS :Not enough paramaters!";
		sendMessage(fd, msg);
		return;
	}
	if (args[1] != this->_password)
	{
		std::cout << "Password incorrect!" << std::endl;
		std::string msg = ":server 464 " + this->_clients[i].getFullIdenifer() + " PASS :Password incorrect!";
		sendMessage(fd, msg);
		this->clearClient(fd);
		return;
	}
	this->_clients[i].auth(true);
}