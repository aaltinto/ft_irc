#include "../includes/server.hpp"
#include <sys/socket.h>
#include <iostream>
#include <cerrno>
#include <cstring>


void sendMessage(int fd, const std::string message)
{
    std::string msg = message + "\r\n";
    if (send(fd, msg.c_str(), msg.length(), 0) < 0)
    {
        std::cerr << "send() failed!\nError: " << std::strerror(errno) << std::endl;
        return ;
    }
}

void Server::sendTopic(int fd, Channels channel)
{
    Client *client = this->getClient(fd);
    std::string topicMsg;
    if (channel.getTopicName().empty())
        topicMsg = ":ircserv 331 " + client->getFullIdenifer() + " " + channel.getChannelName() + " :No topic is set";
    else
        topicMsg = ":ircserv 332 " + client->getFullIdenifer() + " " + channel.getChannelName() + " :" + channel.getTopicName();
    sendMessage(fd, topicMsg);
}

void Channels::sendMessageToAll(std::string message, int excludeFd)
{
	for(size_t i = 0; i < this->_clients.size(); i++)
	{
		if (excludeFd != this->_clients[i].getFd())
			sendMessage(this->_clients[i].getFd(), message);
	}
	for(size_t i = 0; i < this->_admin.size(); i++)
	{
		if (excludeFd != this->_admin[i].getFd())
			sendMessage(this->_admin[i].getFd(), message);
	}
}

void Server::handleJoin(Client client, Channels channel)
{
    std::string joinMsg = ":" + client.getFullIdenifer() + " JOIN " + channel.getChannelName();
    std::string namesMsg = ":ircserv 353 " + client.getFullIdenifer() + " = " + channel.getChannelName() + " :" + channel.getChannelClients();
    std::string endNamesMsg = ":ircserv 366 " + client.getFullIdenifer() + " " + channel.getChannelName() + " :End of /NAMES list.";

    sendMessage(client.getFd(), joinMsg);
    this->sendTopic(client.getFd(), channel);
    sendMessage(client.getFd(), namesMsg);
    sendMessage(client.getFd(), endNamesMsg);
    std::string broadcastmsg = ":" + client.getNick() + " " + "JOIN " + channel.getChannelName();
    channel.sendMessageToAll(broadcastmsg, client.getFd());
}