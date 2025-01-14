#include "server.hpp"

Server::Server(){}

Server::~Server(){}

Server::Server(Server const &server)
{
	this->setFd(server.getFd());
	this->setIpAddr(server.getIpAdd());
}

Server const &Server::operator=(Server &server)
{
	this->setFd(server.getFd());
	this->setIpAddr(server.getIpAdd());
	return *this;
}

int Server::getFd() const
{
	return this->_fd;
}

std::string Server::getIpAdd() const
{
	return this->_ipAddr;
}

void Server::setFd(int const fd)
{
	this->_fd = fd;
}

void Server::setIpAddr(std::string const ipAddr)
{
	this->_ipAddr = ipAddr;
}
