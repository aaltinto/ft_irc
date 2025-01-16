#include "server.hpp"
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <iostream>

void Server::startServer()
{
	while (this->_signal == false)
	{
		if (poll(&this->_fds[0], this->_fds.size(), -1) == -1 && this->_signal == false)
			throw(std::runtime_error("poll() stoped working"));
		for(size_t i = 0; i < this->_fds.size(); i++)
		{
			if (this->_fds[i].revents & POLLIN)
			{
				if (this->_serverSocketFd == this->_fds[i].fd)
					this->acceptNewClient();
				else
					this->recieveNewData(this->_fds[i].fd);
			}
		}
	}
	closeFds();
}

std::vector<std::string> commandSlicer(char *buff)
{
	std::string strBuff(buff);
	std::vector<std::string> splittedCommands;

	size_t j = 0;
	for (size_t i = 0; i < strBuff.size(); i++)
	{
		if (strBuff[i] == ' ')
		{
			splittedCommands.push_back(strBuff.substr(j, i));
			j = i;
		}
	}
	splittedCommands.push_back(strBuff.substr(j, strBuff.size()));
	
	for (size_t i = 0; i < splittedCommands.size(); i++)
		std::cout << splittedCommands[i] << " amk " << std::endl;
	return splittedCommands;
}

void Server::recieveNewData(int fd)
{
	char buff[1024];
	memset(buff, 0, sizeof(buff));
	ssize_t bytes = recv(fd, buff, sizeof(buff) -1, 0);
	if (bytes <= 0)
	{
		std::cout << "Client disconnected <" << fd << ">" << std::endl;
		for (size_t i = 0; i < this->_fds.size(); i++)
		{
			if (fd == this->_fds[i].fd)
			{
				this->_fds.erase(this->_fds.begin() + i);
				break;
			}
		}
		for (size_t i = 0; i < this->_clients.size(); i++)
		{
			if (fd == this->_clients[i].getFd())
			{
				this->_clients.erase(this->_clients.begin() + i);
				break;
			}
		}
		close(fd);
	}
	else
	{
		buff[bytes] = '\0';
		commandSlicer(buff);
		// std::cout << "Client <" << fd << "> \n Data: \n" << buff << std::endl;
	}
}

void Server::serverSocketCreate()
{
	//create a socket
	this->_serverSocketFd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_serverSocketFd == -1)
		throw(std::runtime_error("Failed to create a socket!"));

	//Setup port for listening
	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(this->_port);
	
	//Set port for nonblock mode
	fcntl(this->_serverSocketFd, F_SETFL, O_NONBLOCK);
	
	//bind the socket with port
	bind(this->_serverSocketFd, (struct sockaddr*)&address, sizeof(address));

	listen(this->_serverSocketFd, SOMAXCONN);

	struct pollfd newPoll;

	newPoll.fd = this->_serverSocketFd;
	newPoll.events = POLLIN;
	newPoll.revents = 0;
	this->_fds.push_back(newPoll);
}

void Server::closeFds()
{
	for (size_t i = 0; i < this->_clients.size(); i++)
	{
		std::cout << "Client <" << this->_clients[i].getFd() << "> Disconnected" << std::endl;
		close(this->_clients[i].getFd());
	}
	if (this->_serverSocketFd != -1)
	{
		std::cout << "Server <" << this->_serverSocketFd << "> Disconnected" << std::endl;
		close(this->_serverSocketFd);
	}
}

void Server::acceptNewClient()
{
	Client newClient;
	struct sockaddr_in clientAdress;
	struct pollfd newPoll;
	socklen_t len = sizeof(clientAdress);

	int recievedFd = accept(this->_serverSocketFd, (sockaddr *)&clientAdress, &len);
	if (recievedFd == -1)
	{
		std::cerr << "accept() failed!" << std::endl;
		return ;
	}
	if (fcntl(recievedFd, F_SETFL, O_NONBLOCK) == -1)
	{
		std::cerr << "fcntl() failed!" << std::endl;
		return ;	
	}

	newPoll.fd = recievedFd;
	newPoll.events = POLLIN;
	newPoll.revents = 0;
	this->_fds.push_back(newPoll);

	newClient.setFd(recievedFd);
	newClient.setIpAddr(inet_ntoa(clientAdress.sin_addr));
	this->_clients.push_back(newClient);
	std::cout << "Client connected.\nRecieved fd <" << recievedFd << ">" << std::endl;
}
