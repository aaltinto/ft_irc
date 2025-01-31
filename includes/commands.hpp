#pragma once

#include <vector>
#include <string>
#include "../includes/client.hpp"
#include "../includes/channel.hpp"
class Commands
{
	private:

	public:
		static void join(std::vector<std::string> args, Client client);
		static void topic(std::vector<std::string> args, Client client);
};

void Commands::join(std::vector<std::string> args, Client client)
{
	std::string channelName;
}