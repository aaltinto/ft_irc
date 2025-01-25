#include "server.hpp"
#include "client.hpp"
#include "chanel.hpp"
#include <iostream>

void Server::addChanel(std::string chanelName, Client &client)
{
	Chanels chanel(client, chanelName);
	std::cout << "Chanel created" << std::endl;
	std::cout << "Chanel name: " << chanel.getChanelName() << std::endl;
	this->_chanels.push_back(chanel);
	// std::cout << "Client joined " <<  << std::endl;
}
