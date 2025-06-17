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
			this->closeFds();
			throw std::runtime_error(std::strerror(errno));
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

std::vector<std::string> Server::splitCommands(const std::string& buffer)
{
	std::vector<std::string> commands;
	std::string current;
	
	for (size_t i = 0; i < buffer.size(); i++)
	{
		if (buffer[i] == '\r' && i + 1 < buffer.size() && buffer[i + 1] == '\n')
		{
			if (!current.empty())
			{
				commands.push_back(current);
				current.clear();
			}
			i++; // Skip the \n
		}
		else if (buffer[i] == '\n')
		{
			if (!current.empty())
			{
				commands.push_back(current);
				current.clear();
			}
		}
		else
		{
			current += buffer[i];
		}
	}
	
	// If there's remaining data without CRLF, it's incomplete
	if (!current.empty())
	{
		commands.push_back(current);
	}
	
	return commands;
}

std::vector<std::string> Server::parseCommand(const std::string& command)
{
	std::vector<std::string> tokens;
	std::string current;
	bool inTrailing = false;
	
	for (size_t i = 0; i < command.size(); i++)
	{
		if (command[i] == ':' && !inTrailing && (i == 0 || command[i-1] == ' '))
		{
			if (!current.empty())
			{
				tokens.push_back(current);
				current.clear();
			}
			inTrailing = true;
			if (i + 1 < command.size())
				i++; // Skip the ':'
		}
		
		if (inTrailing)
		{
			if (i < command.size())
				current += command[i];
		}
		else if (command[i] == ' ')
		{
			if (!current.empty())
			{
				tokens.push_back(current);
				current.clear();
			}
		}
		else
		{
			current += command[i];
		}
	}
	
	if (!current.empty())
		tokens.push_back(current);
	
	return tokens;
}

void Server::processCommands(int fd, const std::string& data)
{
	// Add data to client buffer
	this->_clientBuffers[fd] += data;
	
	// Split into complete commands
	std::vector<std::string> commands = this->splitCommands(this->_clientBuffers[fd]);
	
	// Process all complete commands (all but possibly the last one)
	for (size_t i = 0; i < commands.size(); i++)
	{
		// Check if this is the last command and if it ends with CRLF or LF
		bool isComplete = false;
		if (i == commands.size() - 1)
		{
			// Check if the original buffer ends with CRLF or LF
			size_t bufferLen = this->_clientBuffers[fd].size();
			if (bufferLen >= 2 && 
				this->_clientBuffers[fd].substr(bufferLen - 2) == "\r\n")
				isComplete = true;
			else if (bufferLen >= 1 && 
					this->_clientBuffers[fd][bufferLen - 1] == '\n')
				isComplete = true;
		}
		else
		{
			isComplete = true;
		}
		
		if (isComplete)
		{
			std::vector<std::string> tokens = this->parseCommand(commands[i]);
			if (!tokens.empty())
			{
				std::cout << "Processing command from fd <" << fd << ">: " << commands[i] << std::endl;
				this->exec_command(fd, tokens);
			}
		}
		else
		{
			// Keep incomplete command in buffer
			this->_clientBuffers[fd] = commands[i];
			return;
		}
	}
	
	// Clear buffer if all commands were processed
	this->_clientBuffers[fd].clear();
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
	if (commando.empty())
		return;
		
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
	ssize_t bytes = recv(fd, buff, sizeof(buff) - 1, 0);
	
	if (bytes <= 0)
	{
		if (bytes == 0)
			std::cout << "Client <" << fd << "> disconnected (EOF)" << std::endl;
		else
			std::cout << "Client <" << fd << "> disconnected (error)" << std::endl;
		this->clearClient(fd);
	}
	else
	{
		buff[bytes] = '\0';
		std::string data(buff);
		this->processCommands(fd, data);
	}
}

void Server::serverSocketCreate()
{
	//create a socket
	this->_serverSocketFd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_serverSocketFd == -1)
		throw std::runtime_error(std::strerror(errno) + std::string(" - Failed to create socket!"));

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
		throw std::runtime_error("Failed to set nonblock mode!");
	}
	
	//bind the socket with port
	if (bind(this->_serverSocketFd, (struct sockaddr*)&address, sizeof(address)) == -1)
	{
		std::cerr << "bind() failed! " << std::strerror(errno) << std::endl;
		close(this->_serverSocketFd);
		throw std::runtime_error("Failed to bind the socket!");
	}

	if (listen(this->_serverSocketFd, SOMAXCONN) == -1)
	{
		std::cerr << "listen() failed! " << std::strerror(errno) << std::endl;
		close(this->_serverSocketFd);
		throw std::runtime_error("Failed to listen the socket!");
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
	
	// Initialize buffer for new client
	this->_clientBuffers[recievedFd] = "";
	
	std::cout << "Client connected.\nRecieved fd <" << recievedFd << ">" << std::endl;
}
