#include "../includes/server.hpp"
#include "../includes/client.hpp"
#include "../includes/channel.hpp"
#include <iostream>

void Server::addChannel(std::string channelName, Client &client)
{
	Channels channel(client, channelName);
	this->_channels.push_back(channel);
	std::cout << "Client joined " << channel.getChannelName() << std::endl;
}
