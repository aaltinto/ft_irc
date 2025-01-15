#include "server.hpp"
#include <sys/socket.h>
#include <stdexcept>
#include <netinet/in.h>
// #include <sys/types.h>

int isPortValid(int port)
{
	
}

int isPasswordValid(std::string password)
{
	
}

Server::Server(int port, std::string password){
	if (isPortValid(port))
		throw (std::runtime_error("Port number is not valid!"));
	this->_port = port;
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
}

Server const &Server::operator=(Server &server)
{
	return *this;
}

void Server::serverInit()
{
	

}
