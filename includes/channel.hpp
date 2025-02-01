#pragma once
#include <string>
#include <vector>
#include "../includes/client.hpp"

class Channels
{
	private:
		int _topic;
		std::string _name;
		std::string _created_at;
		std::string _create_time;
		std::string _topic_name;
		std::vector<Client> _clients;
		std::vector<Client> _admins;
		
	public:
		Channels(Client &client, std::string channelName);
		Channels(Channels const &channels);
		Channels const &operator=(Channels const &Channels);
		~Channels();
		
		void joinChannel(Client &client);

		std::string getChannelName() const;
		void setChannelName(std::string channelName);

		std::string getChannelClients();
		void sendMessageToAll(std::string message, int excludeFd = 0);
		
		std::string getTopicName();
		void setTopicName(std::string topicName);
		
		void setTime();
		std::string getTime();
};
