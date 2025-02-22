#include "../includes/channel.hpp"
#include "../includes/server.hpp"
#include <iostream>

Channels::Channels(Client &client, std::string channelName): _name(channelName)
{
	client.setAdmin();
	this->_clients.push_back(client);
	this->_topic_name = "";
	this->_password = "";
	this->_mods = "";
	this->_inviteOnly = false;
	this->_topicProtection = false;
	this->_IsProtected = false;
	this->_limit = -1;

}

Channels::~Channels()
{
	
}

Channels::Channels(Channels const &channels)
{
	this->_name = channels._name;
	this->_created_at = channels._created_at;
	this->_create_time = channels._create_time;
	this->_topic_name = channels._topic_name;
	this->_mods = channels._mods;
	this->_password = channels._password;
	this->_inviteOnly = channels._inviteOnly;
	this->_topicProtection = channels._topicProtection;
	this->_IsProtected = channels._IsProtected;
	this->_limit = channels._limit;
	this->_invitedClients = channels._invitedClients;
	this->_clients = channels._clients;
}

Channels const &Channels::operator=(Channels const &channels)
{
	if (this == &channels)
	return *this;
	
	this->_name = channels._name;
	this->_created_at = channels._created_at;
	this->_create_time = channels._create_time;
	this->_topic_name = channels._topic_name;
	this->_password = channels._password;
	this->_mods = channels._mods;
	this->_inviteOnly = channels._inviteOnly;
	this->_topicProtection = channels._topicProtection;
	this->_IsProtected = channels._IsProtected;
	this->_limit = channels._limit;
	this->_invitedClients = channels._invitedClients;
	this->_clients = channels._clients;

	return *this;
}

Client *Channels::getClient(int fd)
{
	for (size_t i = 0; i < this->_clients.size(); i++)
	{
		if (fd == this->_clients[i].getFd())
			return &this->_clients[i];
	}
	return NULL;
}

std::string Channels::getMods() const
{
	if (this->_mods.empty())
		return "There is no mode avaible";
	return this->_mods;
}

void Channels::joinChannel(Client &client)
{
	for (size_t i = 0; i < this->_clients.size(); i++)
		if (client.getFd() == this->_clients[i].getFd())
			return;
	this->_clients.push_back(client);
	std::cout << "Client " << client.getFd() << " joined " << this->getChannelName() <<std::endl;
}

void Channels::partChannel(Client &client)
{
	for (size_t i = 0; i < this->_clients.size(); i++)
	{
		if (client.getFd() == this->_clients[i].getFd())
		{
			this->_clients.erase(this->_clients.begin() + i);
			std::cout << "Client " << client.getFd() << " left " << this->getChannelName() <<std::endl;
			return;
		}
	}
}

void Channels::setPass(std::string pass)
{
	this->_password = pass;
}

std::string Channels::getPass() const
{
	return this->_password;
}

void Channels::adminOps(Client &client, bool value)
{
	for (size_t i = 0; i < this->_clients.size(); i++)
	{
		if (client.getFd() == this->_clients[i].getFd())
		{
			this->_clients[i].setAdmin(value);
			return;
		}
	}
}

void Channels::addInvitedClient(std::string nickname)
{
	this->_invitedClients.push_back(nickname);
}

void Channels::removeInvitedClient(std::string nickname)
{
	for (size_t i = 0; i < this->_invitedClients.size(); i++)
	{
		if (nickname == this->_invitedClients[i])
		{
			this->_invitedClients.erase(this->_invitedClients.begin() + i);
			return;
		}
	}
}

std::string Channels::getChannelName() const
{
	return this->_name;
}

void Channels::setChannelName(std::string channelName)
{
	this->_name = channelName;
}

std::string Channels::getTopicName()
{
	return this->_topic_name;
}

void Channels::setTopicName(std::string topicName)
{
	this->_topic_name = topicName;
}

std::string Channels::getChannelClients()
{
	std::string clients;
	for (size_t i = 0; i < this->_clients.size(); i++)
	{
		if (i != 0)
			clients += " ";
		if (this->_clients[i].isAdmin())
		{
			clients += "@" + this->_clients[i].getNick();
			continue;	
		}
		clients += this->_clients[i].getNick();
	}
	return clients;
}

int Channels::getClientByNick(std::string nickname)
{
	for (size_t i = 0; i < this->_clients.size(); i++)
	{
		if (nickname == this->_clients[i].getNick())
			return this->_clients[i].getFd();
	}
	return -1;
}

bool Channels::checkClientIsIn(int fd)
{
	for (size_t i = 0; i < this->_clients.size(); i++)
	{
		if (fd == this->_clients[i].getFd())
			return 1;
	}
	return 0;
}

int Channels::isAdmin(int fd)
{
	for (size_t i = 0; i < this->_clients.size(); i++)
	{
		if (fd == this->_clients[i].getFd() && this->_clients[i].isAdmin())
			return 1;
	}
	return 0;
}

bool Channels::isProtected()
{
	return _IsProtected;
}

int Channels::getClientIndex(int fd)
{
	for (size_t i = 0; i < this->_clients.size(); i++)
	{
		if (fd == this->_clients[i].getFd())
			return i;
	}
	return -1;
}

bool Channels::isTopicProtected()
{
	return this->_topicProtection;
}

bool Channels::isInvited(std::string nickname)
{
	for (size_t i = 0; i < this->_invitedClients.size(); i++)
	{
		if (nickname == this->_invitedClients[i])
			return 1;
	}
	return 0;
}

bool Channels::isInviteOnly()
{
	return this->_inviteOnly;
}

int Channels::checkLimitExceeded()
{
	if (this->_clients.size() < this->_limit
		|| this->_limit <= 0)
		return 0;
	return 1;
}

void Channels::setLimit(int limit)
{
	this->_limit = limit;
}

int Channels::getClientCount()
{
	return this->_clients.size();
}