#include "chanel.hpp"


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

