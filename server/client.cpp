#include "../includes/client.hpp"

Client::Client(){}

Client::~Client(){}

Client::Client(Client const &client)
{
	this->setFd(client.getFd());
	this->setIpAddr(client.getIpAdd());
	this->setNick(client.getNick());
	this->setRealName(client.getRealName());
	this->setUsername(client.getUsername());
}

Client const &Client::operator=(Client const &client)
{
	this->setFd(client.getFd());
	this->setIpAddr(client.getIpAdd());
	this->setNick(client.getNick());
	this->setRealName(client.getRealName());
	this->setUsername(client.getUsername());

	return *this;
}

int Client::getFd() const
{
	return this->_fd;
}

std::string Client::getIpAdd() const
{
	return this->_ipAddr;
}

std::string Client::getNick() const
{
	return this->_nick;
}

std::string Client::getRealName() const
{
	return this->_realName;
}

std::string Client::getUsername() const
{
	return this->_username;
}

std::string Client::getFullIdenifer() const
{
	return this->_nick + "!" + this->_username + "@" + this->_ipAddr;
}

void Client::setFd(int const fd)
{
	this->_fd = fd;
}

void Client::setIpAddr(std::string const ipAddr)
{
	this->_ipAddr = ipAddr;
}

void Client::setNick(std::string const nick)
{
	this->_nick = nick;
}


void Client::setRealName(std::string const realName)
{
	this->_realName = realName;
}

void Client::setUsername(std::string const username)
{
	this->_username = username;
}

void Client::addChannel(std::string const channel)
{
	this->_joinedChannels.push_back(channel);
}

void Client::removeChannel(std::string const channel)
{
	for (size_t i = 0; i < this->_joinedChannels.size(); i++)
	{
		if (this->_joinedChannels[i] == channel)
		{
			this->_joinedChannels.erase(this->_joinedChannels.begin() + i);
			return;
		}
	}
}

std::vector<std::string> Client::getJoinedChannels() const
{
	return this->_joinedChannels;
}