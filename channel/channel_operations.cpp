#include "../includes/server.hpp"
#include "../includes/client.hpp"
#include "../includes/channel.hpp"
#include "../includes/mode.hpp"
#include <iostream>

void Server::addChannel(std::string channelName, Client client)
{
	if (client.isAuth() == false)
	{
		std::cout << "Password required!" << std::endl;
		passwordRequired(&client, NULL);
		this->clearClient(client.getFd());
		return;
	}
	Channels channel(client, channelName);
	this->_channels.push_back(channel);
	this->handleJoin(client, channel);
	std::cout << "Client " << client.getFd() << " joined " << channel.getChannelName() << std::endl;
}

// irc numeric replies
// https://gist.github.com/proxypoke/2264878
void Server::join(std::vector<std::string> args, int fd)
{
	Client *client = this->getClient(fd);
	if (!client)
	{
		std::cout << "Client not found!" << std::endl;
		return;
	}
	if (client->isAuth() == false)
	{
		std::cout << "Client not authed!" << std::endl;
		passwordRequired(this->getClient(fd), NULL);
		this->clearClient(fd);
		return;
	}
	if (args.size() <= 1)
		return notEnoughParameters(client, "JOIN");
	if (args[1][0] != '#')
	{
		std::cout << "Channel name has to start with '#'" << std::endl;
		return invalidChannelName(client, args[1]);
	}

	std::cout << "join" << std::endl;
	Channels *channel = this->getChannelbyName(args[1]);
	if (channel != NULL)
	{
		if (channel->isInviteOnly() && !channel->isInvited(client->getNick()))
			return inviteOnlyChannel(client, channel);

		if (args.size() < 3 && channel->isProtected())
			return passwordRequired(client, channel);
		if (channel->isProtected() && channel->getPass() != args[2])
			return passwordIncorrect(client, channel);
		if (channel->checkLimitExceeded())
			return channelLimitExceeded(client, channel);
		channel->joinChannel(*client);
		this->handleJoin(*client, *channel);
		client->addChannel(args[1]);
		if (channel->isInviteOnly())
			channel->removeInvitedClient(client->getNick());
		return;
	}

	this->addChannel(args[1], *client);
}

void Server::nick(std::vector<std::string> args, int fd)
{

	std::cout << "nick" << std::endl;
	if (args.size() < 2)
		return notEnoughParameters(this->getClient(fd), "NICK");
	int i = this->getClientIndex(fd);
	if (i == -1)
		return;
	if (this->_clients[i].isAuth() == false)
	{
		std::cout << "Password required!" << std::endl;
		passwordRequired(this->getClient(fd), NULL);
		this->clearClient(fd);
		return;
	}
	this->_clients[i].setNick(args[1]);
}

void Server::user(std::vector<std::string> args, int fd)
{
	std::cout << "user" << std::endl;
	if (args.size() < 5)
		return notEnoughParameters(this->getClient(fd), "USER");

	int i = this->getClientIndex(fd);
	if (i == -1)
		return;
	if (this->_clients[i].isAuth() == false)
	{
		std::cout << "Password required!" << std::endl;
		passwordRequired(this->getClient(fd), NULL);
		this->clearClient(fd);
		return;
	}
	this->_clients[i].setUsername(args[1]);
	this->_clients[i].setRealName(args[4]);
}


void Server::privmsg(std::vector<std::string> args, int fd)
{
	std::cout << "messgae" << std::endl;
	if (args.size() < 3)
		return notEnoughParameters(this->getClient(fd), "PRIVMSG");
	Client *client = this->getClient(fd);
	if (!client)
		return;
	if (client->isAuth() == false)
	{
		std::cout << "Password required!" << std::endl;
		passwordRequired(this->getClient(fd), NULL);
		this->clearClient(fd);
		return;
	}
	std::string myMSG = ":" + client->getFullIdenifer() + " PRIVMSG " + args[1] + " :" + args[2];

	if (args[1][0] == '#')
	{
		Channels *channel = this->getChannelbyName(args[1]);
		if (!channel)
			return noSuchChannel(this->getClient(fd), args[1]);
		if (channel->checkClientIsIn(fd) == false)
			return notInThatChannel(this->getClient(fd), *channel);
		return channel->sendMessageToAll(myMSG, fd);
	}
	int client_fd = this->getClientbyNick(args[1]);
	if (client_fd == -1)
		return noSuchNick(this->getClient(fd), args[1]);
	sendMessage(client_fd, myMSG);
}

void Server::topic(std::vector<std::string> args, int fd)
{
	std::cout << "topic" << std::endl;
	if (args.size() < 2)
		return notEnoughParameters(this->getClient(fd), "TOPIC");
	Client *client = this->getClient(fd);
	if (!client)
		return;
	if (client->isAuth() == false)
	{
		std::cout << "Password required!" << std::endl;
		passwordRequired(this->getClient(fd), NULL);
		this->clearClient(fd);
		return;
	}
	Channels *channel = this->getChannelbyName(args[1]);
	if (!channel)
		return noSuchChannel(this->getClient(fd), args[1]);
	if (channel->checkClientIsIn(fd) == false)
		return notInThatChannel(this->getClient(fd), *channel);
	if (args.size() == 2)
		return this->sendTopic(fd, *channel);
	if (channel->isTopicProtected() && !channel->isAdmin(fd))
	{
		std::cout << "Permission denied!" << std::endl;
		return permissionDenied(this->getClient(fd), *channel);
	}
	channel->setTopicName(args[2]);
	std::string topicUpdate = ":" + client->getFullIdenifer() + " TOPIC " + channel->getChannelName() + " :" + args[2];
	channel->sendMessageToAll(topicUpdate);
}

void Server::part(std::vector<std::string> args, int fd)
{
	std::cout << "part" << std::endl;
	if (args.size() < 2)
		return notEnoughParameters(this->getClient(fd), "PART");
	Client *client = this->getClient(fd);
	if (!client)
		return;
	if (client->isAuth() == false)
	{
		std::cout << "Password required!" << std::endl;
		passwordRequired(this->getClient(fd), NULL);
		this->clearClient(fd);
		return;
	}
	Channels *channel = this->getChannelbyName(args[1]);
	if (!channel)
		return noSuchChannel(this->getClient(fd), args[1]);
	if (channel->checkClientIsIn(fd) == false)
		return notInThatChannel(this->getClient(fd), *channel);
	std::string partMessage;
	if (args.size() == 3)
		partMessage = ":" + client->getFullIdenifer() + " PART " + channel->getChannelName() + " :" + args[2];
	channel->sendMessageToAll(partMessage);
	client->removeChannel(args[1]);
	channel->partChannel(*client);
	if (channel->getClientCount() == 0)
		this->removeChannel(channel->getChannelName());
}



void Server::quit(std::vector<std::string> args, int fd)
{
	std::cout << "quit" << std::endl;
	if (args.size() < 2)
		return notEnoughParameters(this->getClient(fd), "QUIT");
	int i = this->getClientIndex(fd);
	if (i == -1)
		return;
	std::vector<std::string> channels = this->_clients[i].getJoinedChannels();
	this->clearClient(fd, args[1]);
	for (size_t i = 0; i < channels.size(); i++)
	{
		Channels *channel = this->getChannelbyName(channels[i]);
		if (!channel)
			continue;
		if (channel->getClientCount() == 0)
			this->removeChannel(channels[i]);
	}
}

void Server::kick(std::vector<std::string> args, int fd)
{
	std::cout << "kick" << std::endl;
	if (args.size() < 3)
		return notEnoughParameters(this->getClient(fd), "KICK");
	
	Client *kicker = this->getClient(fd);
	if (!kicker)
		return;
	if (kicker->isAuth() == false)
	{
		std::cout << "Password required!" << std::endl;
		passwordRequired(this->getClient(fd), NULL);
		this->clearClient(fd);
		return;
	}
	Channels *channel = this->getChannelbyName(args[1]);
	if (!channel)
		return noSuchChannel(this->getClient(fd), args[1]);
	if (channel->checkClientIsIn(fd) == false)
		return notInThatChannel(this->getClient(fd), *channel);
	if (!channel->isAdmin(fd))
	{
		std::cout << "Permission denied!" << std::endl;
		return permissionDenied(this->getClient(fd), *channel);
	}
	int client_fd = channel->getClientByNick(args[2]);
	if (client_fd == -1)
		return noSuchNick(this->getClient(fd), args[2], channel->getChannelName());
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
		passwordRequired(this->getClient(fd), NULL);
		this->clearClient(fd);
		return;
	}
	if (args[1] != this->_password)
	{
		std::cout << "Password incorrect!" << std::endl;
		passwordIncorrect(this->getClient(fd), NULL);
		this->clearClient(fd);
		return;
	}
	this->_clients[i].auth(true);
}

void Server::invite(std::vector<std::string> args, int fd)
{
	std::cout << "invite" << std::endl;
	if (args.size() < 3)
		return notEnoughParameters(this->getClient(fd), "INVITE");
	Client *inviter = this->getClient(fd);
	if (!inviter)
		return;
	if (inviter->isAuth() == false)
	{
		std::cout << "Client not authed!" << std::endl;
		return;
	}
	int clientFd = this->getClientbyNick(args[1]);
	if (clientFd == -1)
	{
		std::cout << "No such nick!" << std::endl;
		return noSuchNick(this->getClient(fd), args[1], args[2]);
	}
	Channels *channel = this->getChannelbyName(args[2]);
	if (!channel)
	{
		std::cout << "No such channel!" << std::endl;
		return noSuchChannel(this->getClient(fd), args[2]);
	}
	if (channel->checkClientIsIn(fd) == false)
	{
		std::cout << "Not in that channel!" << std::endl;
		return notInThatChannel(this->getClient(fd), *channel);
	}
	if (channel->isTopicProtected() && !channel->isAdmin(fd))
		return permissionDenied(this->getClient(fd), *channel);
	channel->addInvitedClient(args[1]);
	this->_channels[this->getChannelIndex(args[2])] = *channel;
	//store the invitee's fd for check in if the channel invite only
	std::string inviteMsg = ":" + inviter->getFullIdenifer() + " INVITE " + args[1] + " " + args[2];
	sendMessage(clientFd, inviteMsg);
}

static void printModes(Client client, std::string channel, std::string mode)
{
	std::string modeMsg = ":ircserv 324 " + client.getFullIdenifer() + " " + channel + " " + mode;
	sendMessage(client.getFd(), modeMsg);
}

std::string getArg(int i, std::vector<std::string> &args)
{
	for (size_t j = i; j < args.size(); j++)
	{
		if (!args[j].empty() && args[j][0] != '+' && args[j][0] != '-')
		{
			std::string ret = args[j];
			args[j] = "";
			return ret;
		}
	}
	return "";
}

std::vector<Mode> Server::modeSlasher(std::vector<std::string> &args)
{
	std::vector<Mode> modes;

	for (size_t i = 2; i < args.size(); i++)
	{
		if (args[i][0] != '+' && args[i][0] != '-')
			continue;
		for (size_t j = 1; j < args[i].length(); j++)
		{
			Mode mode;
			mode.setSign(args[i][0] == '-' ? -1 : 1);
			mode.setFlag(args[i][j]);
			if (args[i][j] == 'k' || args[i][j] == 'l' || args[i][j] == 'o')
				mode.setArg(getArg(i + 1, args));
			else
				mode.setArg("");
			modes.push_back(mode);
			std::cout << "Mode: " << mode.getSign() << mode.getFlag() << " " << mode.getArg() << std::endl; 
		}
	}
	return modes;
}

void Server::mode(std::vector<std::string> args, int fd)
{
	std::cout << "mode" << std::endl;
	if (args.size() < 2)
		return notEnoughParameters(this->getClient(fd), "MODE");
	Client *client = this->getClient(fd);
	if (!client)
		return ;
	Channels *channel = this->getChannelbyName(args[1]);
	if (!channel)
	{
		std::cerr << "No such channel" << std::endl;
		return noSuchChannel(this->getClient(fd), args[1]);
	}
	if (channel->checkClientIsIn(fd) == false)
	{
		std::cerr << "Not in that channel" << std::endl;
		return notInThatChannel(this->getClient(fd), *channel);
	}
	if (args.size() == 2 || args[2].empty())
		return printModes(*client, args[1], channel->getMods());
	if (!channel->isAdmin(client->getFd()))
	{
		std::cout << "Permission denied!" << std::endl;
		return permissionDenied(this->getClient(fd), *channel);
	}
	std::vector<Mode> modes = modeSlasher(args);
	if (!modes.empty())
		channel->setMode(modes, fd);
}
