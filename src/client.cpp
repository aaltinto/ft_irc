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