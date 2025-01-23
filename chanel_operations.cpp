#include "server.hpp"
#include "client.hpp"
#include "chanel.hpp"
#include <iostream>

void Server::addChanel(std::string chanelName, Client &client)
{
	Chanels chanel(client, chanelName);
	this->_chanels.push_back(chanel);
	std::cout << "Client joined " <<  << std::endl;
}
