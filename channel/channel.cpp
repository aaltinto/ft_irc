#include "../includes/channel.hpp"
#include <iostream>

Channels::Channels(Client &client, std::string channelName): _name(channelName)
{
	this->_clients.push_back(client);
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
	(void)channels;
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

