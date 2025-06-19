#include "../includes/server.hpp"
#include <stdexcept>
#include <signal.h>
#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <cstring>

static int isPortValid(std::string port)
{
	for (size_t i = 0; i < port.length(); i++)
		if (!isdigit(port[i]))
			throw std::runtime_error("Port number contains non-digit characters!");
	int tmpPort = std::atoi(port.c_str());
	if (tmpPort < 1024 || tmpPort > 65535)
		throw std::runtime_error("Port number is not in range 1024-65535!");
	return tmpPort;
}

static int isPasswordValid(std::string password)
{
	if (password.length() < 1 || password.length() > 16)
		throw std::runtime_error("Password length is should be in range 1-16!");
	return 0;
}

int Server::command_in_command = -1;
bool Server::_signal = false;
void Server::signalHandler(int signum)
{
	std::cout << "\n\033[36m╭─────────────────────────────────────────╮\033[0m" << std::endl
          << "\033[36m│\033[0m \033[1;33mServer Signal\033[0m \033[1;36m#" << signum << "\033[0m \033[1;33mreceived\033[0m" << std::endl
          << "\033[36m│\033[0m \033[1;31mShutting down gracefully...\033[0m" << std::endl
          << "\033[36m╰─────────────────────────────────────────╯\033[0m" << std::endl;
	Server::_signal = true;
}

Server::Server(std::string port, std::string password){
	
	isPasswordValid(password);
	this->_password = password;
	this->_port = isPortValid(port);
	this->serverSocketCreate();
}


Server::~Server(){
}

Server::Server(Server const &server)
{
	this->_password = server._password;
	this->_port = server._port;
	this->_serverSocketFd = server._serverSocketFd;
	this->_signal = server._signal;

	this->_clients = server._clients;
	this->_fds = server._fds;
	this->_channels = server._channels;
	this->_clientBuffers = server._clientBuffers;
}

Server const &Server::operator=(Server &server)
{
	if (this == &server)
    	return *this;

	this->_password = server._password;
	this->_port = server._port;
	this->_serverSocketFd = server._serverSocketFd;
	this->_signal = server._signal;

	this->_clients = server._clients;
	this->_fds = server._fds;
	this->_channels = server._channels;
	this->_clientBuffers = server._clientBuffers;

	return *this;
}

int Server::getChannelIndex(std::string channelName)
{
	for (size_t i = 0; i < this->_channels.size(); i++)
	{
		if (this->_channels[i].getChannelName() == channelName)
			return i;
	}
	return -1;
}

Client *Server::getClient(int fd)
{
	for (size_t i = 0; i < this->_clients.size(); i++)
	{
		if (this->_clients[i].getFd() == fd)
		{

			return &this->_clients[i];
		}
	}
	return NULL;
}

int Server::getClientIndex(int fd)
{
	for (size_t i = 0; i < this->_clients.size(); i++)
	{
		if (this->_clients[i].getFd() == fd)
			return i;
	}
	return -1;
}

Channels* Server::getChannelbyName(std::string channelName)
{
	for (size_t i = 0; i < this->_channels.size(); i++)
	{
		if (this->_channels[i].getChannelName() == channelName)
			return &this->_channels[i];
	}
	return NULL;
}

int Server::getClientbyNick(std::string nickname)
{
	for (size_t i = 0; i < this->_clients.size(); i++)
	{
		if (this->_clients[i].getNick() == nickname)
			return this->_clients[i].getFd();
	}
	return -1;
}

void Server::clearClient(int fd, std::string quitMsg)
{
	int index = this->getClientIndex(fd);
	if (index == -1)
		throw std::runtime_error("Client not found");
	std::string quitMessage = ":" + this->_clients[index].getFullIdenifer() + " QUIT :" + quitMsg;
	std::vector<std::string> joinedChannels = this->_clients[index].getJoinedChannels();
	for (size_t i = 0; i < joinedChannels.size(); i++)
	{
		int j = this->getChannelIndex(joinedChannels[i]);
		this->_channels[j].sendMessageToAll(quitMessage, fd);
		this->_channels[j].partChannel(this->_clients[index]);
		if (this->_channels[j].getClientCount() == 0)
		{
			std::cout << "\033[36m╭─────────────────────────────────────────╮\033[0m" << std::endl
          << "\033[36m│\033[0m \033[1;34mChannel\033[0m \033[1;36m" << _channels[j].getChannelName() << "\033[0m" << std::endl
          << "\033[36m│\033[0m \033[1;31mEmpty channel removed\033[0m" << std::endl
          << "\033[36m╰─────────────────────────────────────────╯\033[0m" << std::endl;
			this->removeChannel(this->_channels[j].getChannelName());
		}
	}
	this->_clients.erase(this->_clients.begin() + index);
	for (size_t i = 0; i < this->_fds.size(); i++)
	{
		if (fd == this->_fds[i].fd)
		{
			this->_fds.erase(this->_fds.begin() + i);
			break;
		}
	}
	this->_clientBuffers.erase(fd);
	close(fd);
}

void Server::removeChannel(std::string channelName)
{
	int index = this->getChannelIndex(channelName);
	if (index == -1)
		return ;
	this->_channels.erase(this->_channels.begin() + index);
}
