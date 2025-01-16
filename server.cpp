#include "server.hpp"
#include <sys/socket.h>
#include <stdexcept>
#include <netinet/in.h>
#include <sys/types.h>

int isPortValid(std::string port)
{
	for (size_t i = 0; i < port.length(); i++)
		if (!isdigit(port[i]))
			throw(std::runtime_error("Port number contains non-digit characters!"));
	int tmpPort = std::stoi(port.c_str());
	// 0-1023 are reserved ports.
	if (tmpPort < 1024 || tmpPort > 65535)
		throw(std::runtime_error("Port number is not in range 1024-65535!"));
	return tmpPort;
}

int isPasswordValid(std::string password)
{
	if (password.length() < 1 || password.length() > 16)
		throw(std::runtime_error("Password length is should be in range 1-16!"));
	return 0;
}

Server::Server(std::string port, std::string password){
	int tmpPort = isPortValid(port);
	this->_port = tmpPort;
	isPasswordValid(password);
	this->_password = password;

	//create a socket
	this->_serverSocketFd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_serverSocketFd != 0)
		throw(std::runtime_error("Failed to create a socket!"));

	//Setup port for listening
	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(this->_port);
	
	//bind the socket with port
	bind(this->_serverSocketFd, (struct sockaddr*)&address, sizeof(address));
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

void Server::serverInit()
{
	

}
