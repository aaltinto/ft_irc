#include "chanel.hpp"
#include <iostream>

Chanels::Chanels(Client &client, std::string chanelName)
{
	this->_clients.push_back(client);
	this->_admins.push_back(client);
	this->setChanelName(chanelName);
	this->_topic = 0;
}

Chanels::~Chanels()
{
	
}

Chanels::Chanels(Chanels const &chanels)
{
	(void)chanels;
}

Chanels const &Chanels::operator=(Chanels const &chanels)
{
	(void)chanels;
	return *this;
}

void Chanels::joinChanel(Client &client)
{
	for (size_t i = 0; i < this->_clients.size(); i++)
		if (client.getFd() == this->_clients[i].getFd())
			return;
	this->_clients.push_back(client);
	std::cout << "Client " << client.getFd() << " joined " << this->getChanelName() <<std::endl;
}

std::string Chanels::getChanelName()
{
	return this->_name;
}

void Chanels::setChanelName(std::string chanelName)
{
	this->_name = chanelName;
}

std::string Chanels::getTopicName()
{
	return this->_topic_name;
}

void Chanels::setTopicName(std::string topicName)
{
	this->_topic_name = topicName;
}

