#include "../includes/server.hpp"
#include <sys/socket.h>
#include <iostream>

void sendMessage(int fd, const std::string message)
{
    std::string msg = message + "\r\n";
    if (send(fd, msg.c_str(), msg.length(), 0) < 0)
    {
        std::cerr << "send() failed!" << std::endl;
        return ;
    }
}

void Server::handleJoin(Client &client, Channels &channel)
{
    std::string joinMsg = ":" + client.getUsername() + " JOIN " + channel.getChannelName();
    std::cout << joinMsg << std::endl;

    std::string topicMsg;
    if (channel.getTopicName().empty())
        topicMsg = ":yourserver 331 " + client.getUsername() + " " + channel.getChannelName() + " :No topic is set";
    else
        topicMsg = ":yourserver 332 " + client.getUsername() + " " + channel.getChannelName() + " :" + channel.getTopicName();

    std::string namesMsg = ":yourserver 353 " + client.getUsername() + " = " + channel.getChannelName() + " :" + channel.getChannelClients();
    std::string endNamesMsg = ":yourserver 366 " + client.getUsername() + " " + channel.getChannelName() + " :End of /NAMES list.";

    sendMessage(client.getFd(), joinMsg);
    sendMessage(client.getFd(), topicMsg);
    sendMessage(client.getFd(), namesMsg);
    sendMessage(client.getFd(), endNamesMsg);
}