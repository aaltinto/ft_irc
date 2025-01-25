#include "server.hpp"
#include <stdexcept>
#include <signal.h>
#include <cstdlib>
#include <iostream>

static int isPortValid(std::string port)
{
	for (size_t i = 0; i < port.length(); i++)
		if (!isdigit(port[i]))
			throw(std::runtime_error("Port number contains non-digit characters!"));
	int tmpPort = std::atoi(port.c_str());
	// 0-1023 are reserved ports.
	if (tmpPort < 1024 || tmpPort > 65535)
		throw(std::runtime_error("Port number is not in range 1024-65535!"));
	return tmpPort;
}

static int isPasswordValid(std::string password)
{
	if (password.length() < 1 || password.length() > 16)
		throw(std::runtime_error("Password length is should be in range 1-16!"));
	return 0;
}

bool Server::_signal = false;
void Server::signalHandler(int signum)
{
	std::cout << "Signal recieved: " << signum << std::endl;
	Server::_signal = true;
}

Server::Server(std::string port, std::string password){
	int tmpPort = isPortValid(port);
	this->_port = tmpPort;
	isPasswordValid(password);
	this->_password = password;
	this->serverSocketCreate();
}


Server::~Server(){}

Server::Server(Server const &server)
{
	(void)server;
}

Server const &Server::operator=(Server &server)
{
	(void)server;
	return *this;
}

Client &Server::getClient(int fd)
{
	for (size_t i = 0; i < this->_clients.size(); i++)
	{
		if (this->_clients[i].getFd() == fd)
			return this->_clients[i];
	}
	throw std::runtime_error("Client couldn't found");
}

void Server::join(std::vector<std::string> args, int fd)
{
	std::string chanelName;
	if (args.size() <= 1)
		return;
	std::cout << "args size: "<< args.size() << "\n";
	if (args[1][0] != '#')
	{
		std::cout << "Chanel name has to start with '#'" << std::endl;
		return;
	}
	chanelName = args[1];

	if (this->_chanels.size() != 0)
	{
		for (size_t i = 0; i <= this->_chanels.size(); i++)
		{
			std::cout << std::endl;
			std::cout << "_chanels: " << this->_chanels[i].getChanelName() << std::endl << "chanelName: " << chanelName << std::endl;
			std::cout << std::endl;
			if(this->_chanels[i].getChanelName() == chanelName)
			{
				this->_chanels[i].joinChanel(this->getClient(fd));
				std::cout << "joinChanel\n";
				return ;
			}
		}
	}
	std::cout << "addChanel\n";
	this->addChanel(chanelName, this->getClient(fd));

}