#include "../includes/server.hpp"
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

std::vector<std::string> commandSlicer(char *buff, int start)
{
	std::string strBuff(buff);
	std::vector<std::string> splittedCommands;

	size_t j = start;
	for (size_t i = start; i < strBuff.size(); i++)
	{
		if (strBuff[i] == 32 || strBuff[i] == 10)
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

int detectCommands(std::vector<std::string> commands)
{
	for (size_t i = 0; i < commands.size(); i++)
	{
		std::cout << "Command["<< i << "]: " << commands[i] << std::endl;
	}
	std::string list[] = {"join", "nick", "user", "privmsg", "topic", "quit"};
	std::string lowercommand = to_lower(commands[0]);
	size_t list_size = sizeof(list) / sizeof(list[0]);
	for (size_t i = 0; i < list_size; i++)
	{
		if (lowercommand == list[i])
		{
			return i;
		}
	}
	return -1;
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
		std::vector<std::string> commands = commandSlicer(buff, 0);
		int max = 1;
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
			switch (detectCommands(commando[i]))
			{
				case 0:
					std::cout << "-join-\n"; 
					this->join(commando[i], fd);
					break;
				case 1:
					std::cout << "-nick-\n"; 
					this->nick(commando[i], fd);
					break;
				case 2:
					std::cout << "-user-\n"; 
					this->user(commando[i], fd);
					break;
				case 3:
					std::cout << "-msg-\n"; 
					this->privmsg(commando[i], fd);
					break;
				case 4:
					std::cout << "-topic-\n"; 
					this->topic(commando[i], fd);
					break;
				default:
					break;
			}
		}
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
