#include "client.hpp"

Client::Client(){}

Client::~Client(){}

Client::Client(Client const &client)
{
	this->setFd(client.getFd());
	this->setIpAddr(client.getIpAdd());
}

Client const &Client::operator=(Client const &client)
{
	this->setFd(client.getFd());
	this->setIpAddr(client.getIpAdd());
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

void Client::setFd(int const fd)
{
	this->_fd = fd;
}

void Client::setIpAddr(std::string const ipAddr)
{
	this->_ipAddr = ipAddr;
}
