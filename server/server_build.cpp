#include "../includes/server.hpp"
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <cerrno>
#include <cstring>

void Server::startServer()
{
	while (this->_signal == false)
	{
		if (poll(&this->_fds[0], this->_fds.size(), -1) == -1 && this->_signal == false)
		{
			if (errno == EINTR)
				continue;
			this->closeFds();
			throw(std::runtime_error(std::strerror(errno)));
		}
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

std::vector<std::string> commandSlicer(char *buff, int start)
{
	std::string strBuff(buff);
	std::vector<std::string> splittedCommands;

	size_t j = start;
	for (size_t i = start; i < strBuff.size(); i++)
	{
		if (strBuff[i] == 32)
		{
			splittedCommands.push_back(strBuff.substr(j, i - j));
			j = i + 1;
		}
		if (strBuff[i] == 13 && i + 1 < strBuff.size() && strBuff[i + 1])
		{
			splittedCommands.push_back(strBuff.substr(j, i - j));
			Server::command_in_command = i + 2;
			return splittedCommands;
		}
		if (strBuff[i] == ':')
		{
			splittedCommands.push_back(strBuff.substr(j + 1, strBuff.size() -j));
			return splittedCommands;
		}
	}
	splittedCommands.push_back(strBuff.substr(j, strBuff.size() -j-1));
	
	return splittedCommands;
}

std::string to_lower(std::string str)
{
	std::string ret(str);
	for (size_t i = 0; i < str.size(); i++)
	{
		if (str[i] == '/')
			continue;
		ret[i] = std::tolower(str[i]);
	}
	return ret;
}

void Server::exec_command(int fd, std::vector<std::string> commando)
{
	std::string list[] = {
		"join",
		"pass",
		"nick",
		"user",
		"privmsg",
		"topic",
		"invite",
		"kick",
		"part",
		"mode",
		"quit"
		};
	int list_size = sizeof(list) / sizeof(list[0]);
	void (Server::*commandsFuncs[])(std::vector<std::string>, int) = {
		&Server::join,
		&Server::pass,
		&Server::nick,
		&Server::user,
		&Server::privmsg,
		&Server::topic,
		&Server::invite,
		&Server::kick,
		&Server::part,
		&Server::mode,
		&Server::quit
	};

	for (size_t j = 0; j < commando.size(); j++)
		std::cout << "command[" << j << "]: " << commando[j] << std::endl;
	std::string lowerCommand = to_lower(commando[0]);
	int cmd = 0;
	while (cmd < list_size && lowerCommand != list[cmd])
		cmd++;
	if (cmd < list_size)
		(this->*commandsFuncs[cmd])(commando, fd);
}

void Server::recieveNewData(int fd)
{
	char buff[1024];
	memset(buff, 0, sizeof(buff));
	ssize_t bytes = recv(fd, buff, sizeof(buff) -1, 0);
	if (bytes <= 0)
	{
		std::cout << "Client disconnected <" << fd << ">" << std::endl;
		this->clearClient(fd);
	}
	else
	{
		buff[bytes] = '\0';
		int max = 1;

		std::vector<std::string> commands = commandSlicer(buff, 0);
		std::vector<std::vector<std::string> > commando;
		commando.push_back(commands);

		for (int i = 0; i < max; i++)
		{
			if (Server::command_in_command != -1)
			{
				int k = Server::command_in_command;
				max++;
				commando.push_back(commandSlicer(buff, Server::command_in_command));
				if (Server::command_in_command == k)
					Server::command_in_command = -1;
			}
			this->exec_command(fd, commando[i]);
		}
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
	if (fcntl(this->_serverSocketFd, F_SETFL, O_NONBLOCK) == -1)
	{
		std::cerr << "fcntl() failed! " << std::strerror(errno) << std::endl;
		close(this->_serverSocketFd);
		throw(std::runtime_error("Failed to set nonblock mode!"));
	}
	
	//bind the socket with port
	if (bind(this->_serverSocketFd, (struct sockaddr*)&address, sizeof(address)) == -1)
	{
		std::cerr << "bind() failed! " << std::strerror(errno) << std::endl;
		close(this->_serverSocketFd);
		throw(std::runtime_error("Failed to bind the socket!"));
	}

	if (listen(this->_serverSocketFd, SOMAXCONN) == -1)
	{
		std::cerr << "listen() failed! " << std::strerror(errno) << std::endl;
		close(this->_serverSocketFd);
		throw(std::runtime_error("Failed to listen the socket!"));
	}

	struct pollfd newPoll;

	newPoll.fd = this->_serverSocketFd;
	newPoll.events = POLLIN;
	newPoll.revents = 0;
	this->_fds.push_back(newPoll);
}

void Server::closeFds()
{
	for (size_t i = 0; i < this->_clients.size(); i++)
		this->clearClient(this->_clients[i].getFd());
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
		std::cerr << "accept() failed! " << std::strerror(errno) << std::endl;
		if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)
			return ;
		this->closeFds();
		throw(std::runtime_error("Critical error! Shutting down"));
	}
	if (fcntl(recievedFd, F_SETFL, O_NONBLOCK) == -1)
	{
		std::cerr << "fcntl() failed! " << std::strerror(errno) << std::endl;
		close(recievedFd);
		return ;	
	}

	newPoll.fd = recievedFd;
	newPoll.events = POLLIN;
	newPoll.revents = 0;
	this->_fds.push_back(newPoll);

	newClient.setFd(recievedFd);
	char *ip = inet_ntoa(clientAdress.sin_addr);
	if (!ip)
	{
		std::cerr << "inet_ntoa() failed! " << std::endl;
		close(recievedFd);
		return ;
	}
	newClient.setIpAddr(ip);
	this->_clients.push_back(newClient);
	std::cout << "Client connected.\nRecieved fd <" << recievedFd << ">" << std::endl;
}
