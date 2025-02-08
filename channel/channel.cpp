#include "../includes/channel.hpp"
#include "../includes/server.hpp"
#include <iostream>

Channels::Channels(Client &client, std::string channelName): _name(channelName)
{
	this->_admins.push_back(client);
	this->_topic = 0;
}

Channels::~Channels()
{
	
}

Channels::Channels(Channels const &channels)
{
	this->_name = channels._name;
	this->_topic = channels._topic;
	this->_created_at = channels._created_at;
	this->_create_time = channels._create_time;
	this->_topic_name = channels._topic_name;
	this->_clients = channels._clients;
	this->_admins = channels._admins;
}

Channels const &Channels::operator=(Channels const &channels)
{
	if (this == &channels)
		return *this;
	
	this->_name = channels._name;
	this->_topic = channels._topic;
	this->_created_at = channels._created_at;
	this->_create_time = channels._create_time;
	this->_topic_name = channels._topic_name;

	this->_clients = channels._clients;
	this->_admins = channels._admins;

	return *this;
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
	for (size_t i = 0; i < this->_admins.size(); i++)
	{
		if (client.getFd() == this->_admins[i].getFd())
		{
			this->_admins.erase(this->_admins.begin() + i);
			std::cout << "Client " << client.getFd() << " left " << this->getChannelName() <<std::endl;
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
	for (size_t j = 0; j < this->_admins.size(); j++)
	{
		if (j == this->_admins.size())
			clients += "@" + this->_admins[j].getNick();
		else
			clients += "@" + this->_admins[j].getNick() + " ";
	}
	for (size_t i = 0; i < this->_clients.size(); i++)
	{
		if (i == this->_clients.size())
			clients += this->_clients[i].getNick();
		else
			clients += this->_clients[i].getNick() + " ";
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
	for (size_t i = 0; i < this->_admins.size(); i++)
	{
		if (nickname == this->_admins[i].getNick())
			return this->_admins[i].getFd();
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
	for (size_t i = 0; i < this->_admins.size(); i++)
	{
		if (fd == this->_admins[i].getFd())
			return 1;
	}
	return 0;
}

int Channels::isAdmin(int fd)
{
	for (size_t i = 0; i < this->_admins.size(); i++)
	{
		if (fd == this->_admins[i].getFd())
			return 1;
	}
	return 0;
}